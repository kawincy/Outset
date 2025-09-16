/*
  ==============================================================================

    EnvComp.h
    Created: 2 Feb 2025 12:46:52pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class EnvComp : public juce::Component, private juce::Slider::Listener
{
public:
    EnvComp(int num, juce::AudioProcessorValueTreeState& apvtsRef);
    ~EnvComp() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void initializeSlider(juce::Slider& slider, const juce::String& name, double min, double max, double interval, double initialValue, bool skewed);
    void sliderValueChanged(juce::Slider* slider) override;
    void setSliderBounds(juce::Slider& slider, juce::Label& label, juce::Rectangle<int> bounds);
public:
  // Accessors for current envelope values
  float getAttack()  const { return (float) attackSlider.getValue(); }
  float getDecay()   const { return (float) decaySlider.getValue(); }
  float getSustain() const { return (float) sustainSlider.getValue(); }
  float getRelease() const { return (float) releaseSlider.getValue(); }

  // Set all envelope parameters at once (no notification to avoid recursive callbacks)
  void setAll(float a, float d, float s, float r)
  {
    attackSlider.setValue(a, juce::dontSendNotification);
    decaySlider.setValue(d, juce::dontSendNotification);
    sustainSlider.setValue(s, juce::dontSendNotification);
    releaseSlider.setValue(r, juce::dontSendNotification);
    repaint();
  }

  std::function<void(float attack, float decay, float sustain, float release)> onEnvChange; // callback for external sync
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    
    int envNum;
    juce::AudioProcessorValueTreeState& apvtsRef;
    
    juce::AudioProcessorValueTreeState::SliderAttachment attackAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment decayAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment sustainAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment releaseAttachment;
    


    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvComp)
};


