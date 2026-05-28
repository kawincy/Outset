/*
  ==============================================================================

    rta.h - Simple real-time spectrum analyzer (header-only)
    Created: 19 Oct 2025

    Captures audio from the audio thread, performs windowed FFT, and exposes
    a smoothed magnitude spectrum for rendering on the UI thread.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class RTA
{
public:
    // fftOrder 11 = 2048, 12 = 4096, etc.
    explicit RTA(int fftOrder = 11)
        : fftOrder(fftOrder),
          fftSize(1 << fftOrder), // bit shift for fast 2^N
          fft(fftOrder),
          window(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris, false)
    {
        timeDomainBuffer.resize((size_t)fftSize);
        fftBuffer.resize((size_t)fftSize * 2, 0.0f); // complex buffer (real/imag interleaved for JUCE FFT)
        magnitudeBuffer.resize((size_t)fftSize / 2, -100.0f);
        smoothedMagnitude.resize((size_t)fftSize / 2, -100.0f);
    }

    void setSampleRate(double sr) { sampleRate.store(sr, std::memory_order_relaxed); }

    // Push interleaved channel pointers from processBlock
    void pushAudioBuffer(const float* const* channels, int numChannels, int numSamples)
    {
        if (numChannels <= 0 || numSamples <= 0) return;

        // Downmix to mono and push into FIFO
        for (int i = 0; i < numSamples; ++i)
        {
            float s = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
                s += channels[ch][i];
            s *= (1.0f / juce::jmax(1, numChannels));
            pushSample(s);
        }
    }

    // Copies the current smoothed magnitude (dB) into out; out.size() will be set accordingly
    void getSmoothedSpectrum(juce::Array<float>& out) const
    {
        const juce::ScopedLock sl(magLock);
        out.clearQuick();
        out.resize(smoothedMagnitude.size());
        for (size_t i = 0; i < smoothedMagnitude.size(); ++i)
            out.setUnchecked((int)i, smoothedMagnitude[i]);
    }

    // Direct access for custom drawing without Array allocation
    void copySmoothedSpectrum(std::vector<float>& out) const
    {
        const juce::ScopedLock sl(magLock);
        out = smoothedMagnitude;
    }

    int getFftSize() const noexcept { return fftSize; }
    double getSampleRate() const noexcept { return sampleRate.load(std::memory_order_relaxed); }

private:
    void pushSample(float s)
    {
        fifo[(size_t)fifoIndex] = s;

        if (++fifoIndex >= fftSize)
        {
            fifoIndex = 0;
            // Copy fifo to timeDomainBuffer in order
            for (int i = 0; i < fftSize; ++i)
                timeDomainBuffer[(size_t)i] = fifo[(size_t)i];

            performFft();
        }
    }

    void performFft()
    {
        // Window
        window.multiplyWithWindowingTable(timeDomainBuffer.data(), fftSize);

        // Prepare FFT buffer (real in, imag 0)
        std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
        for (int i = 0; i < fftSize; ++i)
            fftBuffer[(size_t)i] = timeDomainBuffer[(size_t)i];

        // Real-only forward transform in-place
        fft.performRealOnlyForwardTransform(fftBuffer.data());

        // Compute magnitudes for first half of spectrum
        const float falloff = 0.85f; // smoothing
        const float floorDb = -90.0f;

        {
            const juce::ScopedLock sl(magLock);
            for (int i = 0; i < fftSize / 2; ++i)
            {
                float real = fftBuffer[(size_t)i * 2];
                float imag = fftBuffer[(size_t)i * 2 + 1];
                float mag = std::sqrt(real * real + imag * imag) / (float)fftSize;
                float db = juce::Decibels::gainToDecibels(mag, floorDb);

                // Smooth
                smoothedMagnitude[(size_t)i] = juce::jmax(db, falloff * smoothedMagnitude[(size_t)i] + (1.0f - falloff) * db);
            }
        }
    }

    // Members
    const int fftOrder;
    const int fftSize;
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;

    std::atomic<double> sampleRate { 48000.0 };

    // Simple FIFO buffer for mono samples
    std::vector<float> fifo { std::vector<float>((size_t)fftSize, 0.0f) };
    int fifoIndex = 0;

    std::vector<float> timeDomainBuffer;
    std::vector<float> fftBuffer; // real/imag interleaved per JUCE FFT

    mutable juce::CriticalSection magLock;
    std::vector<float> magnitudeBuffer;     // instantaneous (unused externally)
    std::vector<float> smoothedMagnitude;   // exposed to UI
};
