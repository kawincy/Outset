/*
  ==============================================================================

    PresetsComp.cpp
    Created: 2 Feb 2025 12:37:21pm
    Author:  josep

  ==============================================================================
*/

#include <JuceHeader.h>

#include "Colors.h"
#include "PresetsComp.h"

//==============================================================================
PresetsComp::PresetsComp(PresetManager& pm) :
    presetPanel(pm)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(presetPanel);
}

PresetsComp::~PresetsComp()
{
}

void PresetsComp::paint (juce::Graphics& g)
{
    g.fillAll(colors().bg);

    g.setColour(colors().main);
    g.setFont(juce::FontOptions(14.0f));
    g.drawText("Presets", getLocalBounds(), juce::Justification::centred, true);
}

void PresetsComp::resized()
{
    presetPanel.setBounds(getBounds());
}
