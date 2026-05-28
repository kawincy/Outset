/*
  ==============================================================================

    DelayNode.cpp

  ==============================================================================
*/

#include "DelayNode.h"

//==============================================================================
DelayNode::DelayNode()
{
    // Initialize with default parameters
    delayTimeMs = 250.0f;
    feedback = 0.3f;
    mix = 0.3f;
    lowPassCutoff = 8000.0f;
}

//==============================================================================
void DelayNode::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;
    
    // Prepare delay lines
    for (auto& delayLine : delayLines)
    {
        delayLine.prepare(spec);
        delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    }
    
    // Prepare low-pass filters
    for (auto& filter : lowPassFilters)
    {
        filter.prepare(spec);
    }
    
    // Update parameters
    updateDelayTime();
    updateLowPassFilter();
    
    // Reset state
    reset();
}

void DelayNode::reset()
{
    // Clear delay lines
    for (auto& delayLine : delayLines)
    {
        delayLine.reset();
    }
    
    // Reset filters
    for (auto& filter : lowPassFilters)
    {
        filter.reset();
    }
}

//==============================================================================
void DelayNode::setDelayTime(float timeMs)
{
    delayTimeMs = juce::jlimit(0.0f, 2000.0f, timeMs);
    updateDelayTime();
}

void DelayNode::setFeedback(float feedbackAmount)
{
    feedback = juce::jlimit(0.0f, 0.95f, feedbackAmount);
}

void DelayNode::setMix(float mixValue)
{
    mix = juce::jlimit(0.0f, 1.0f, mixValue);
}

void DelayNode::setLowPassCutoff(float cutoffHz)
{
    lowPassCutoff = juce::jlimit(200.0f, 20000.0f, cutoffHz);
    updateLowPassFilter();
}

//==============================================================================
void DelayNode::updateDelayTime()
{
    delayTimeInSamples = (delayTimeMs / 1000.0f) * static_cast<float>(currentSampleRate);
    delayTimeInSamples = juce::jlimit(0.0f, static_cast<float>(maxDelayInSamples), delayTimeInSamples);
}

void DelayNode::updateLowPassFilter()
{
    if (currentSampleRate > 0.0)
    {
        auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
            currentSampleRate, lowPassCutoff);
        
        for (auto& filter : lowPassFilters)
        {
            filter.coefficients = coefficients;
        }
    }
}

//==============================================================================
template<typename ProcessContext>
void DelayNode::process(const ProcessContext& context) noexcept
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
        const auto* inputData = (context.usesSeparateInputAndOutputBlocks()) 
            ? inputBlock.getChannelPointer(channel) 
            : channelData;

        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            // Get delayed sample
            float delayedSample = delayLines[channel].popSample(0, delayTimeInSamples, true);
            
            // Apply low-pass filter to feedback
            float filteredFeedback = lowPassFilters[channel].processSample(delayedSample);
            
            // Calculate input to delay line (input + filtered feedback)
            float delayInput = inputData[sample] + (filteredFeedback * feedback);
            
            // Push new sample to delay line
            delayLines[channel].pushSample(0, delayInput);
            
            // Mix dry and wet signals
            channelData[sample] = inputData[sample] * (1.0f - mix) + delayedSample * mix;
        }
    }
}

// Explicit template instantiations for common ProcessContext types
template void DelayNode::process<juce::dsp::ProcessContextReplacing<float>>(const juce::dsp::ProcessContextReplacing<float>&) noexcept;
template void DelayNode::process<juce::dsp::ProcessContextNonReplacing<float>>(const juce::dsp::ProcessContextNonReplacing<float>&) noexcept;
