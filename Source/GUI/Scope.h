/*
  ==============================================================================

    Scope.h
    Created: 1 Apr 2025 2:47:40pm
    Author:  ryanb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class Scope : public juce::Component, private juce::Timer
{
public:
    Scope()
    {
        startTimerHz(24); // Update rate in Hz
        backgroundColor = juce::Colour::fromRGBA(0, 0, 0, 70);
        visColor = juce::Colours::white;
        readIndex = 0;
    }

    ~Scope() override
    {
        stopTimer();
    }

    // Sets new audio data for visualization.
    void setAudioData(const juce::AudioBuffer<float>& buffer)
    {
        // Uncomment the ScopedLock if you add a critical section for thread safety
        // const juce::ScopedLock sl(lock);
        audioData.makeCopyOf(buffer);
        readIndex = 0;
    }

    // Paints the waveform.
    void paint(juce::Graphics& g) override
    {
        auto area = getLocalBounds().toFloat();
        g.setColour(backgroundColor);
        g.fillRect(area); // Draw background

        g.setColour(visColor);
        auto yCenter = area.getCentreY();

        // Uncomment the ScopedLock if thread safety is required
        // const juce::ScopedLock sl(lock);
        for (int i = 0; i < audioData.getNumSamples(); ++i)
        {
            float sample = 0.0f;
            for (int ch = 0; ch < audioData.getNumChannels(); ++ch)
                sample += audioData.getSample(ch, i);
            sample /= float(audioData.getNumChannels());

            float x = juce::jmap<float>(i, 0, audioData.getNumSamples(), area.getX(), area.getRight());
            float y = juce::jmap<float>(sample, -1.0f, 1.0f, area.getBottom(), area.getY());
            g.drawLine(x, yCenter, x, y, 2.0f);
        }
    }

    // Timer callback to repaint the component regularly.
    void timerCallback() override
    {
        repaint();
    }

private:
    juce::AudioBuffer<float> audioData;
    int readIndex = 0;
    juce::Colour backgroundColor, visColor;

    // Uncomment and add if thread safety becomes necessary:
    // juce::CriticalSection lock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scope)
};
