/*
  ==============================================================================
    ThreeBandEQNode.h
  ==============================================================================
*/
#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

//==============================================================================
/**
    A DSP processor node that implements a three-band parametric equalizer.

    Bands are controlled via drag points (frequency, gain) in the style of ReaEQ,
    rather than discrete gain/freq knobs.

    Band layout:
      - Band 0: Low shelf   (freq: 20–500 Hz,    gain: ±12 dB)
      - Band 1: Peak/bell   (freq: 200–5000 Hz,  gain: ±12 dB, Q: 0.1–10)
      - Band 2: High shelf  (freq: 2000–20000 Hz, gain: ±12 dB)
*/
class ThreeBandEQNode
{
public:
    //==============================================================================
    /** Represents one draggable EQ band. */
    struct BandPoint
    {
        float freqHz  = 1000.0f;   ///< Center / corner frequency
        float gainDb  = 0.0f;      ///< Gain in dB  (-12 … +12)
        float q       = 1.0f;      ///< Q factor – only meaningful for mid band
    };

    //==============================================================================
    ThreeBandEQNode();
    ~ThreeBandEQNode() = default;

    //==============================================================================
    /** Prepares the processor for playback. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets internal filter state. */
    void reset();

    /** Processes audio via the JUCE DSP ProcessContext interface. */
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept;

    //==============================================================================
    // --- Drag-point API (ReaEQ style) ---

    /** Returns a copy of the current state for the given band (0=low, 1=mid, 2=high). */
    BandPoint getBandPoint(int bandIndex) const noexcept;

    /**
     * Sets frequency and gain for a band by dragging its control point.
     * For the mid band (index 1) the Q is left unchanged; use setMidQ separately
     * if you expose a scroll-wheel Q control.
     *
     * @param bandIndex  0 = low shelf, 1 = mid peak, 2 = high shelf
     * @param freqHz     New frequency (clamped to per-band safe range)
     * @param gainDb     New gain in dB (clamped to ±12 dB)
     */
    void setBandPoint(int bandIndex, float freqHz, float gainDb);

    /**
     * Adjusts mid-band Q (e.g. from a scroll-wheel gesture on band 1).
     * @param delta  Additive delta applied to current Q before clamping to [0.1, 10].
     */
    void adjustMidQ(float delta);

    //==============================================================================
    // --- Low-level scalar setters (kept for backwards compatibility) ---
    void setLowGain(float gainDb);
    void setLowFreq(float freqHz);
    void setMidGain(float gainDb);
    void setMidFreq(float freqHz);
    void setMidQ(float qValue);
    void setHighGain(float gainDb);
    void setHighFreq(float freqHz);

private:
    //==============================================================================
    static constexpr int maxChannels = 8;

    std::array<juce::dsp::IIR::Filter<float>, maxChannels> lowShelfFilters;
    std::array<juce::dsp::IIR::Filter<float>, maxChannels> midFilters;
    std::array<juce::dsp::IIR::Filter<float>, maxChannels> highShelfFilters;

    BandPoint lowBand  { 200.0f,   0.0f, 0.707f };
    BandPoint midBand  { 1000.0f,  0.0f, 1.0f   };
    BandPoint highBand { 8000.0f,  0.0f, 0.707f };

    double currentSampleRate = 44100.0;

    void updateLowShelfFilter();
    void updateMidFilter();
    void updateHighShelfFilter();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeBandEQNode)
};
