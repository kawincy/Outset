/*
  ==============================================================================

    KeyboardComp.cpp
    Created: 2 Feb 2025 12:49:37pm
    Author:  josep

  ==============================================================================
*/

#include <JuceHeader.h>
#include "KeyboardComp.h"

//==============================================================================
KeyboardComp::KeyboardComp(juce::MidiKeyboardState& ks ) : keyboardState(ks), keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    keyboardComponent.setWantsKeyboardFocus(false);
    addAndMakeVisible (keyboardComponent);
}

KeyboardComp::~KeyboardComp()
{
}

void KeyboardComp::paint (juce::Graphics& g)
{
//    g.fillAll(juce::Colours::lightgrey);
//
//    g.setColour(juce::Colours::black);
//    g.drawRect(getLocalBounds(), 1);
//
//    g.setColour(juce::Colours::black);
//    g.setFont(juce::FontOptions(14.0f));
//    g.drawText("Keyboard", getLocalBounds(), juce::Justification::centred, true);
}

void KeyboardComp::resized()
{
    keyboardComponent.setBounds(getLocalBounds());
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
