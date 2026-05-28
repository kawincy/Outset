/*
  ==============================================================================
    ThreeBandEQNode.cpp
  ==============================================================================
*/

#include "ThreeBandEQNode.h"

//==============================================================================
ThreeBandEQNode::ThreeBandEQNode()
{
    // Defaults are set in the BandPoint member initialisers.
}

//==============================================================================
void ThreeBandEQNode::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    for (auto& f : lowShelfFilters)  f.prepare(spec);
    for (auto& f : midFilters)       f.prepare(spec);
    for (auto& f : highShelfFilters) f.prepare(spec);

    updateLowShelfFilter();
    updateMidFilter();
    updateHighShelfFilter();

    reset();
}

void ThreeBandEQNode::reset()
{
    for (auto& f : lowShelfFilters)  f.reset();
    for (auto& f : midFilters)       f.reset();
    for (auto& f : highShelfFilters) f.reset();
}

//==============================================================================
// Drag-point API
//==============================================================================

ThreeBandEQNode::BandPoint ThreeBandEQNode::getBandPoint(int bandIndex) const noexcept
{
    switch (bandIndex)
    {
        case 0:  return lowBand;
        case 1:  return midBand;
        case 2:  return highBand;
        default: jassertfalse; return {};
    }
}

void ThreeBandEQNode::setBandPoint(int bandIndex, float freqHz, float gainDb)
{
    gainDb = juce::jlimit(-12.0f, 12.0f, gainDb);

    switch (bandIndex)
    {
        case 0:
            lowBand.freqHz = juce::jlimit(20.0f, 500.0f, freqHz);
            lowBand.gainDb = gainDb;
            updateLowShelfFilter();
            break;

        case 1:
            midBand.freqHz = juce::jlimit(200.0f, 5000.0f, freqHz);
            midBand.gainDb = gainDb;
            updateMidFilter();
            break;

        case 2:
            highBand.freqHz = juce::jlimit(2000.0f, 20000.0f, freqHz);
            highBand.gainDb = gainDb;
            updateHighShelfFilter();
            break;

        default:
            jassertfalse;
            break;
    }
}

void ThreeBandEQNode::adjustMidQ(float delta)
{
    midBand.q = juce::jlimit(0.1f, 10.0f, midBand.q + delta);
    updateMidFilter();
}

//==============================================================================
// Legacy scalar setters
//==============================================================================

void ThreeBandEQNode::setLowGain(float gainDb)
{
    setBandPoint(0, lowBand.freqHz, gainDb);
}

void ThreeBandEQNode::setLowFreq(float freqHz)
{
    setBandPoint(0, freqHz, lowBand.gainDb);
}

void ThreeBandEQNode::setMidGain(float gainDb)
{
    setBandPoint(1, midBand.freqHz, gainDb);
}

void ThreeBandEQNode::setMidFreq(float freqHz)
{
    setBandPoint(1, freqHz, midBand.gainDb);
}

void ThreeBandEQNode::setMidQ(float qValue)
{
    midBand.q = juce::jlimit(0.1f, 10.0f, qValue);
    updateMidFilter();
}

void ThreeBandEQNode::setHighGain(float gainDb)
{
    setBandPoint(2, highBand.freqHz, gainDb);
}

void ThreeBandEQNode::setHighFreq(float freqHz)
{
    setBandPoint(2, freqHz, highBand.gainDb);
}

//==============================================================================
// Filter coefficient updates
//==============================================================================

void ThreeBandEQNode::updateLowShelfFilter()
{
    if (currentSampleRate <= 0.0) return;

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        currentSampleRate,
        lowBand.freqHz,
        0.707f,
        juce::Decibels::decibelsToGain(lowBand.gainDb));

    for (auto& f : lowShelfFilters)
        f.coefficients = coeffs;
}

void ThreeBandEQNode::updateMidFilter()
{
    if (currentSampleRate <= 0.0) return;

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate,
        midBand.freqHz,
        midBand.q,
        juce::Decibels::decibelsToGain(midBand.gainDb));

    for (auto& f : midFilters)
        f.coefficients = coeffs;
}

void ThreeBandEQNode::updateHighShelfFilter()
{
    if (currentSampleRate <= 0.0) return;

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        currentSampleRate,
        highBand.freqHz,
        0.707f,
        juce::Decibels::decibelsToGain(highBand.gainDb));

    for (auto& f : highShelfFilters)
        f.coefficients = coeffs;
}

//==============================================================================
// Audio processing
//==============================================================================

template<typename ProcessContext>
void ThreeBandEQNode::process(const ProcessContext& context) noexcept
{
    if (context.isBypassed)
    {
        if (context.usesSeparateInputAndOutputBlocks())
            context.getOutputBlock().copyFrom(context.getInputBlock());
        return;
    }

    auto& inputBlock  = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    auto numChannels  = outputBlock.getNumChannels();
    auto numSamples   = outputBlock.getNumSamples();

    if (context.usesSeparateInputAndOutputBlocks())
        outputBlock.copyFrom(inputBlock);

    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto* data = outputBlock.getChannelPointer(ch);

        for (size_t i = 0; i < numSamples; ++i)
        {
            float s = data[i];
            s = lowShelfFilters[ch].processSample(s);
            s = midFilters[ch].processSample(s);
            s = highShelfFilters[ch].processSample(s);
            data[i] = s;
        }
    }
}

template void ThreeBandEQNode::process<juce::dsp::ProcessContextReplacing<float>>(
    const juce::dsp::ProcessContextReplacing<float>&) noexcept;
template void ThreeBandEQNode::process<juce::dsp::ProcessContextNonReplacing<float>>(
    const juce::dsp::ProcessContextNonReplacing<float>&) noexcept;