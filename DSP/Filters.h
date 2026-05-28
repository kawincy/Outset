/*
  ==============================================================================

    Filters.h
    Created: 16 Feb 2025 3:35:53pm
    Author:  ryanb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Filters
{
public:
    // Define our filter types
    enum class FilterType { LowPass, BandPass, HighPass };

    Filters();
    ~Filters();

    // Prepares the filter (call this from your prepareToPlay)
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    // Set filter parameters
    void setType(FilterType type);
    void setCutoffFrequency(float frequencyHz);
    void setResonance(float resonance);

    // Process an entire audio buffer (in-place)
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Optionally process a single sample
    //float processSample(float sample);

private:
    // Using JUCE’s TPT state variable filter (recommended over the older version)
    juce::dsp::StateVariableTPTFilter<float> filter;
};
