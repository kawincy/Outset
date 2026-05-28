/*
  ==============================================================================

    Filters.cpp
    Created: 16 Feb 2025 3:35:53pm
    Author:  ryanb

  ==============================================================================
*/
#include "Filters.h"

Filters::Filters()
{
    // Reset the filter state on construction.
    filter.reset();
}

Filters::~Filters() {}

void Filters::prepare(const juce::dsp::ProcessSpec& spec)
{
    filter.prepare(spec);
}

void Filters::reset()
{
    filter.reset();
}

void Filters::setType(FilterType type)
{
    switch (type)
    {
    case FilterType::LowPass:
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        break;
    case FilterType::BandPass:
        filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        break;
    case FilterType::HighPass:
        filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        break;
    default:
        jassertfalse; // Unknown filter type!
        break;
    }
}

void Filters::setCutoffFrequency(float frequencyHz)
{
    filter.setCutoffFrequency(frequencyHz);
}

void Filters::setResonance(float resonance)
{
    filter.setResonance(resonance);
}

void Filters::processBlock(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
}

//float Filters::processSample(float sample)
//{
//    return filter.processSample(sample);
//}
