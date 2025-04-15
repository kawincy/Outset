/*
  ==============================================================================

    AlgoBoxComp.cpp
    Created: 1 Apr 2025 2:26:29pm
    Author:  josep

  ==============================================================================
*/

#include "AlgoBoxComp.h"
#include "Colors.h"
#include <JuceHeader.h>

AlgoBoxComp::AlgoBoxComp() {
    num_s = "1";
    is_carrier = false;
    exists = false;

    mainBlue = juce::Colour(colors().main);
    accentBlue = juce::Colour(colors().accent);
    offBlue = juce::Colour(colors().carrier_blue);
}

AlgoBoxComp::~AlgoBoxComp() {

}

void AlgoBoxComp::paint(juce::Graphics& g) {
    if (exists) {
        if (is_carrier) {
            g.fillAll(offBlue);
        }
        else {
            g.fillAll(juce::Colour(colors().bg));
        }

        g.setColour(mainBlue);
        g.drawRect(getLocalBounds(), 1);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(14.0f, juce::Font::bold)); // Set font size & style

        auto area = getLocalBounds();  // Get the size of the component

        g.drawText(num_s, area, juce::Justification::centred, true);
    }
}

void AlgoBoxComp::resized()
{

}

void AlgoBoxComp::set_num(int num) {
    num_s = std::to_string(num);
}

void AlgoBoxComp::set_carrier(bool carrier) {
    is_carrier = carrier;
}

void AlgoBoxComp::set_existance(bool e) {
    exists = e;
}