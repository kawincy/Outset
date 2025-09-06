/*
  ==============================================================================

    EnvComp.h
    Created: 2 Feb 2025 12:46:52pm
    Author:  josep, ryanb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscComp : public juce::Component, private juce::Slider::Listener
{
public:
    OscComp(int num, juce::AudioProcessorValueTreeState& apvtsRef);
    ~OscComp() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void initializeSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, juce::Slider::SliderStyle style, double initialValue);
    void initializeSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, juce::Slider::SliderStyle style, double initialValue, double midpointValue);

    void setSliderBounds(juce::Slider& slider, juce::Label& label, juce::Rectangle<int> bounds);
    void drawSliderOutline(juce::Graphics& g, juce::Slider& slider);
    void addLabel(juce::Slider& slider, juce::Label& label, const juce::String& name);
    void sliderValueChanged(juce::Slider* slider) override;
    void updateLabel(juce::Label& label, const juce::String& name, double value);

    int oscNum;
    juce::AudioProcessorValueTreeState& apvtsRef;

    juce::Slider oscLevelSlider;
    juce::Slider oscFineSlider;
    juce::Slider oscCoarseSlider;
    juce::Slider oscRatioSlider;

    juce::AudioProcessorValueTreeState::SliderAttachment levelAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment fineAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment coarseAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment ratioAttachment;

    juce::Label oscLevelLabel;
    juce::Label oscFineLabel;
    juce::Label oscCoarseLabel;
    juce::Label oscRatioLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscComp)
};
