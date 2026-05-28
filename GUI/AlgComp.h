/*
  ==============================================================================

    AlgComp.h
    Created: 2 Feb 2025 12:49:27pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AlgComp  : public juce::Component
{
public:
    AlgComp(juce::AudioProcessorValueTreeState& apvtsRef);
    ~AlgComp() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setAlgIndexParameter(int newValue);


private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    
//    int algo_ind; the new way to access this is with apvtsRef.getRawParameterValue("ALG_INDEX")->load()
    
    std::unique_ptr<juce::DrawableButton> next_b;
    std::unique_ptr<juce::DrawableButton> prev_b;
    juce::Image image;
    juce::Array<juce::Image> images;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlgComp)
};
