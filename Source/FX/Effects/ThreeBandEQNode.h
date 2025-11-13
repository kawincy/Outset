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
    
    This class provides low shelf, parametric mid, and high shelf filters
    while maintaining compatibility with JUCE's DSP framework.
*/
class ThreeBandEQNode
{
public:
    //==============================================================================
    ThreeBandEQNode();
    ~ThreeBandEQNode() = default;

    //==============================================================================
    /** Prepares the processor for playback with the given sample rate and buffer size. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the processor's internal state. */
    void reset();

    /** Processes audio data using the ProcessContext interface. */
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept;

    //==============================================================================
    /** Sets the low band gain in dB (-12 to +12). */
    void setLowGain(float gainDb);
    
    /** Sets the low band frequency in Hz (20-500). */
    void setLowFreq(float freqHz);
    
    /** Sets the mid band gain in dB (-12 to +12). */
    void setMidGain(float gainDb);
    
    /** Sets the mid band frequency in Hz (200-5000). */
    void setMidFreq(float freqHz);
    
    /** Sets the mid band Q factor (0.1-10). */
    void setMidQ(float qValue);
    
    /** Sets the high band gain in dB (-12 to +12). */
    void setHighGain(float gainDb);
    
    /** Sets the high band frequency in Hz (2000-20000). */
    void setHighFreq(float freqHz);

private:
    //==============================================================================
    static constexpr int maxChannels = 8;
    
    std::array<juce::dsp::IIR::Filter<float>, maxChannels> lowShelfFilters;
    std::array<juce::dsp::IIR::Filter<float>, maxChannels> midFilters;
    std::array<juce::dsp::IIR::Filter<float>, maxChannels> highShelfFilters;
    
    float lowGain = 0.0f;
    float lowFreq = 200.0f;
    float midGain = 0.0f;
    float midFreq = 1000.0f;
    float midQ = 1.0f;
    float highGain = 0.0f;
    float highFreq = 8000.0f;
    
    double currentSampleRate = 44100.0;
    
    /** Updates the low shelf filter coefficients. */
    void updateLowShelfFilter();
    
    /** Updates the mid parametric filter coefficients. */
    void updateMidFilter();
    
    /** Updates the high shelf filter coefficients. */
    void updateHighShelfFilter();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeBandEQNode)
};
