/*
  ==============================================================================

    BitCrusherNode.h

  ==============================================================================
*/

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

//==============================================================================
/**
    A DSP processor node that implements bit crushing and sample rate reduction
    for lo-fi digital distortion effects.
    
    This class provides bit depth reduction and sample rate downsampling
    while maintaining compatibility with JUCE's DSP framework.
*/
class BitCrusherNode
{
public:
    //==============================================================================
    BitCrusherNode();
    ~BitCrusherNode() = default;

    //==============================================================================
    /** Prepares the processor for playback with the given sample rate and buffer size. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the processor's internal state. */
    void reset();

    /** Processes audio data using the ProcessContext interface. */
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept;

    //==============================================================================
    /** Sets the bit depth (1-16 bits). */
    void setBitDepth(float depth);
    
    /** Sets the sample rate reduction factor (1-50). */
    void setSampleRateReduction(float reduction);
    
    /** Sets the wet/dry mix (0.0 = dry, 1.0 = wet). */
    void setMix(float mixValue);

private:
    //==============================================================================
    float bitDepth = 16.0f;
    float sampleRateReduction = 1.0f;
    float mix = 0.5f;
    
    // Sample and hold state for each channel
    std::array<float, 8> holdValue{};  // Support up to 8 channels
    std::array<int, 8> sampleCounter{};
    
    double currentSampleRate = 44100.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BitCrusherNode)
};
