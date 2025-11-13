/*
  ==============================================================================

    ReverbNode.h

  ==============================================================================
*/

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>


//==============================================================================
/**
    A DSP processor node that wraps JUCE's Reverb class for use in ProcessorChain.
    
    This class provides the necessary interface methods required by JUCE's DSP
    framework while maintaining compatibility with the existing reverb parameters.
*/
class ReverbNode
{
public:
    //==============================================================================
    ReverbNode();
    ~ReverbNode() = default;

    //==============================================================================
    /** Prepares the processor for playback with the given sample rate and buffer size. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the processor's internal state. */
    void reset();

    /** Processes audio data using the ProcessContext interface. */
    template<typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        // Handle bypassed state
        if (context.isBypassed)
        {
            if (context.usesSeparateInputAndOutputBlocks())
                context.getOutputBlock().copyFrom(context.getInputBlock());
            return;
        }

        // Get audio block from context
        auto& audioBlock = context.getOutputBlock();
        auto numChannels = audioBlock.getNumChannels();
        auto numSamples = audioBlock.getNumSamples();

        // Process based on channel configuration
        if (numChannels == 1)
        {
            // Mono processing
            auto* monoData = audioBlock.getChannelPointer(0);
            reverb.processMono(monoData, static_cast<int>(numSamples));
        }
        else if (numChannels >= 2)
        {
            // Stereo processing
            auto* leftData = audioBlock.getChannelPointer(0);
            auto* rightData = audioBlock.getChannelPointer(1);
            reverb.processStereo(leftData, rightData, static_cast<int>(numSamples));
            
            // Handle any additional output channels (copy from stereo if needed)
            for (int channel = 2; channel < numChannels; ++channel)
            {
                auto* destChannel = audioBlock.getChannelPointer(channel);
                const auto* srcChannel = audioBlock.getChannelPointer(channel % 2);
                juce::FloatVectorOperations::copy(destChannel, srcChannel, static_cast<int>(numSamples));

            }
        }
    }

    //==============================================================================
    /** Updates the reverb parameters. */
    void setParameters(const juce::Reverb::Parameters& newParams);
    
    /** Gets the current reverb parameters. */
    const juce::Reverb::Parameters& getParameters() const noexcept { return currentParams; }
    
    /** Sets individual parameter values. */
    void setRoomSize(float roomSize);
    void setDamping(float damping);
    void setWetLevel(float wetLevel);
    void setDryLevel(float dryLevel);
    void setWidth(float width);
    void setFreezeMode(float freezeMode);
    
    /** Convenience method to set wet/dry mix (0.0 = dry, 1.0 = wet). */
    void setMix(float mix);

private:
    //==============================================================================
    juce::Reverb reverb;
    juce::Reverb::Parameters currentParams;
    double currentSampleRate = 44100.0;
    
    /** Updates the internal reverb with current parameters. */
    void updateInternalReverb();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbNode)
};
