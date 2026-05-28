/*
  ==============================================================================

    KeyboardComp.h
    Created: 2 Feb 2025 12:49:37pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//==============================================================================
/*
*/
class KeyboardComp  : public juce::Component
{
public:
    KeyboardComp(juce::MidiKeyboardState& keyboardState);
    ~KeyboardComp() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    juce::MidiKeyboardState& keyboardState; //this is a reference because we need it to be the same object as the one we construct in the plugin processor
    juce::MidiKeyboardComponent keyboardComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyboardComp)
};

