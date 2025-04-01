/*
  ==============================================================================

    LFOComp.cpp
    Created: 2 Feb 2025 12:49:27pm
    Author:  josep

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LFOComp.h"
//==============================================================================
LFOComp::LFOComp()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    algo_ind = 0;
    
    addAndMakeVisible(test);
    addAndMakeVisible(test2);
    test2.set_carrier(false);
    test2.set_num(2);
    
   
}

LFOComp::~LFOComp()
{
}

void LFOComp::paint (juce::Graphics& g)
{
    juce::Rectangle<int> bounds = getLocalBounds();
    g.fillAll(juce::Colour(0x1A, 0x1A, 0x1A));

    g.setColour(juce::Colour(0x5B, 0x8F, 0x7E));
    g.drawRect(getLocalBounds(), 1);

    // boxes are 4 rows, 6 columns
    
}

void LFOComp::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    juce::Rectangle<int> bounds = getLocalBounds();

    test.setBounds(bounds.getWidth() / 2, bounds.getHeight() / 2, 30, 20);
    test2.setBounds(bounds.getWidth() / 2, bounds.getHeight() / 2 - 30, 30, 20);
}
