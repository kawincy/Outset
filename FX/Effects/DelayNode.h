/*
  ==============================================================================

    DelayNode.h

  ==============================================================================
*/

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

//==============================================================================
/**
    A DSP processor node that implements digital delay with feedback and filtering.
    
    This class provides variable delay time, feedback control, and low-pass filtering
    while maintaining compatibility with JUCE's DSP framework.
*/
class DelayNode
{
public:
    //==============================================================================
    DelayNode();
    ~DelayNode() = default;

    //==============================================================================
    /** Prepares the processor for playback with the given sample rate and buffer size. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the processor's internal state. */
    void reset();

    /** Processes audio data using the ProcessContext interface. */
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept;

    //==============================================================================
    /** Sets the delay time in milliseconds (0-2000ms). */
    void setDelayTime(float timeMs);
    
    /** Sets the feedback amount (0.0-0.95). */
    void setFeedback(float feedbackAmount);
    
    /** Sets the wet/dry mix (0.0 = dry, 1.0 = wet). */
    void setMix(float mixValue);
    
    /** Sets the low-pass filter cutoff frequency for feedback (200-20000Hz). */
    void setLowPassCutoff(float cutoffHz);

private:
    //==============================================================================
    static constexpr int maxDelayInSamples = 96000; // 2 seconds at 48kHz
    static constexpr int maxChannels = 8;
    
    std::array<juce::dsp::DelayLine<float>, maxChannels> delayLines;
    std::array<juce::dsp::IIR::Filter<float>, maxChannels> lowPassFilters;
    
    float delayTimeMs = 250.0f;
    float delayTimeInSamples = 0.0f;
    float feedback = 0.3f;
    float mix = 0.3f;
    float lowPassCutoff = 8000.0f;
    
    double currentSampleRate = 44100.0;
    
    /** Updates the delay time in samples based on current sample rate. */
    void updateDelayTime();
    
    /** Updates the low-pass filter coefficients. */
    void updateLowPassFilter();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayNode)
};
