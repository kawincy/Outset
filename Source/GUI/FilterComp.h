/*
  ==============================================================================

    FilterComp.h
    Created: 2 Feb 2025 12:46:44pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FilterComp : public juce::Component,
    public juce::Slider::Listener
{
public:
    FilterComp(juce::AudioProcessorValueTreeState& apvtsRef);
    ~FilterComp() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Slider callback
    void sliderValueChanged(juce::Slider* slider) override;

    // Optionally, update the sample rate used for response calculation
    void setSampleRate(float newSampleRate) { sampleRate = newSampleRate; repaint(); }

private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    
    juce::AudioProcessorValueTreeState::SliderAttachment cutoffAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment resonanceAttachment;

    juce::Label freqLabel;
    juce::Label qLabel;

    juce::TextEditor cutoffTextBox;
    juce::TextEditor resonanceTextBox;

    // Sample rate used for computing the response curve (default: 48000 Hz)
    // Need to implement some sort of update for this ideally
    float sampleRate = 48000;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterComp)
};

