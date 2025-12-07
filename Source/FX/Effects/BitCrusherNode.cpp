/*
  ==============================================================================

    BitCrusherNode.cpp

  ==============================================================================
*/

#include "BitCrusherNode.h"

//==============================================================================
BitCrusherNode::BitCrusherNode()
{
    // Initialize with default parameters
    bitDepth = 16.0f;
    sampleRateReduction = 1.0f;
    mix = 0.5f;
    
    // Initialize arrays
    holdValue.fill(0.0f);
    sampleCounter.fill(0);
}

//==============================================================================
void BitCrusherNode::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;
    
    // Reset state
    reset();
}

void BitCrusherNode::reset()
{
    // Clear sample and hold state
    holdValue.fill(0.0f);
    sampleCounter.fill(0);
}

//==============================================================================
void BitCrusherNode::setBitDepth(float depth)
{
    bitDepth = juce::jlimit(1.0f, 16.0f, depth);
}

void BitCrusherNode::setSampleRateReduction(float reduction)
{
    sampleRateReduction = juce::jlimit(1.0f, 50.0f, reduction);
}

void BitCrusherNode::setMix(float mixValue)
{
    mix = juce::jlimit(0.0f, 1.0f, mixValue);
}

//==============================================================================
template<typename ProcessContext>
void BitCrusherNode::process(const ProcessContext& context) noexcept
{
    // Handle bypassed state
    if (context.isBypassed)
    {
        if (context.usesSeparateInputAndOutputBlocks())
            context.getOutputBlock().copyFrom(context.getInputBlock());
        return;
    }

    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    auto numChannels = outputBlock.getNumChannels();
    auto numSamples = outputBlock.getNumSamples();

    // Copy input to output first
    if (context.usesSeparateInputAndOutputBlocks())
        outputBlock.copyFrom(inputBlock);

    // Process each channel
    for (size_t channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = outputBlock.getChannelPointer(channel);
        
        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            // Sample rate reduction (sample and hold)
            if (sampleRateReduction > 1.0f)
            {
                if (sampleCounter[channel] >= static_cast<int>(sampleRateReduction))
                {
                    holdValue[channel] = channelData[sample];
                    sampleCounter[channel] = 0;
                }
                else
                {
                    channelData[sample] = holdValue[channel];
                }
                sampleCounter[channel]++;
            }

            // Bit depth reduction
            if (bitDepth < 16.0f)
            {
                float levels = std::pow(2.0f, bitDepth);
                float quantized = std::floor(channelData[sample] * levels + 0.5f) / levels;
                channelData[sample] = quantized;
            }

            // Apply mix
            float drySignal = (context.usesSeparateInputAndOutputBlocks()) 
                ? inputBlock.getChannelPointer(channel)[sample] 
                : channelData[sample];
                
            channelData[sample] = drySignal * (1.0f - mix) + channelData[sample] * mix;
        }
    }
}

// Explicit template instantiations for common ProcessContext types
template void BitCrusherNode::process<juce::dsp::ProcessContextReplacing<float>>(const juce::dsp::ProcessContextReplacing<float>&) noexcept;
template void BitCrusherNode::process<juce::dsp::ProcessContextNonReplacing<float>>(const juce::dsp::ProcessContextNonReplacing<float>&) noexcept;
