/*
  ==============================================================================

    ThreeBandEQNode.cpp

  ==============================================================================
*/

#include "ThreeBandEQNode.h"

//==============================================================================
ThreeBandEQNode::ThreeBandEQNode()
{
    // Initialize with default parameters
    lowGain = 0.0f;
    lowFreq = 200.0f;
    midGain = 0.0f;
    midFreq = 1000.0f;
    midQ = 1.0f;
    highGain = 0.0f;
    highFreq = 8000.0f;
}

//==============================================================================
void ThreeBandEQNode::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;
    
    // Prepare all filters
    for (auto& filter : lowShelfFilters)
    {
        filter.prepare(spec);
    }
    
    for (auto& filter : midFilters)
    {
        filter.prepare(spec);
    }
    
    for (auto& filter : highShelfFilters)
    {
        filter.prepare(spec);
    }
    
    // Update filter coefficients
    updateLowShelfFilter();
    updateMidFilter();
    updateHighShelfFilter();
    
    // Reset state
    reset();
}

void ThreeBandEQNode::reset()
{
    // Reset all filters
    for (auto& filter : lowShelfFilters)
    {
        filter.reset();
    }
    
    for (auto& filter : midFilters)
    {
        filter.reset();
    }
    
    for (auto& filter : highShelfFilters)
    {
        filter.reset();
    }
}

//==============================================================================
void ThreeBandEQNode::setLowGain(float gainDb)
{
    lowGain = juce::jlimit(-12.0f, 12.0f, gainDb);
    updateLowShelfFilter();
}

void ThreeBandEQNode::setLowFreq(float freqHz)
{
    lowFreq = juce::jlimit(20.0f, 500.0f, freqHz);
    updateLowShelfFilter();
}

void ThreeBandEQNode::setMidGain(float gainDb)
{
    midGain = juce::jlimit(-12.0f, 12.0f, gainDb);
    updateMidFilter();
}

void ThreeBandEQNode::setMidFreq(float freqHz)
{
    midFreq = juce::jlimit(200.0f, 5000.0f, freqHz);
    updateMidFilter();
}

void ThreeBandEQNode::setMidQ(float qValue)
{
    midQ = juce::jlimit(0.1f, 10.0f, qValue);
    updateMidFilter();
}

void ThreeBandEQNode::setHighGain(float gainDb)
{
    highGain = juce::jlimit(-12.0f, 12.0f, gainDb);
    updateHighShelfFilter();
}

void ThreeBandEQNode::setHighFreq(float freqHz)
{
    highFreq = juce::jlimit(2000.0f, 20000.0f, freqHz);
    updateHighShelfFilter();
}

//==============================================================================
void ThreeBandEQNode::updateLowShelfFilter()
{
    if (currentSampleRate > 0.0)
    {
        auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            currentSampleRate, lowFreq, 0.707f, juce::Decibels::decibelsToGain(lowGain));
        
        for (auto& filter : lowShelfFilters)
        {
            filter.coefficients = coefficients;
        }
    }
}

void ThreeBandEQNode::updateMidFilter()
{
    if (currentSampleRate > 0.0)
    {
        auto coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            currentSampleRate, midFreq, midQ, juce::Decibels::decibelsToGain(midGain));
        
        for (auto& filter : midFilters)
        {
            filter.coefficients = coefficients;
        }
    }
}

void ThreeBandEQNode::updateHighShelfFilter()
{
    if (currentSampleRate > 0.0)
    {
        auto coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            currentSampleRate, highFreq, 0.707f, juce::Decibels::decibelsToGain(highGain));
        
        for (auto& filter : highShelfFilters)
        {
            filter.coefficients = coefficients;
        }
    }
}

//==============================================================================
template<typename ProcessContext>
void ThreeBandEQNode::process(const ProcessContext& context) noexcept
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
            float inputSample = channelData[sample];
            
            // Process through each filter stage
            float processedSample = inputSample;
            
            // Low shelf filter
            processedSample = lowShelfFilters[channel].processSample(processedSample);
            
            // Mid parametric filter
            processedSample = midFilters[channel].processSample(processedSample);
            
            // High shelf filter
            processedSample = highShelfFilters[channel].processSample(processedSample);
            
            channelData[sample] = processedSample;
        }
    }
}

// Explicit template instantiations for common ProcessContext types
template void ThreeBandEQNode::process<juce::dsp::ProcessContextReplacing<float>>(const juce::dsp::ProcessContextReplacing<float>&) noexcept;
template void ThreeBandEQNode::process<juce::dsp::ProcessContextNonReplacing<float>>(const juce::dsp::ProcessContextNonReplacing<float>&) noexcept;
