/*
  ==============================================================================

    OscEnvParent.h
    Created: 4 Feb 2025 2:22:46pm
    Author:  ryanb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OscComp.h"
#include "EnvComp.h"

class OscEnvParent : public juce::Component
{
public:
    OscEnvParent(const int operatorNum, juce::AudioProcessorValueTreeState& apvtsRef)
        : oscComp(operatorNum, apvtsRef),
          envComp(operatorNum, apvtsRef),
          operatorNum(operatorNum),
          apvtsRef(apvtsRef)
    {
        addAndMakeVisible(oscComp);
        addAndMakeVisible(envComp);
    }

  EnvComp& getEnvComp() noexcept { return envComp; }

    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds = bounds.withTrimmedBottom(30);
        auto oscBounds = bounds.removeFromTop(bounds.getHeight() / 2);
        oscComp.setBounds(oscBounds);
        envComp.setBounds(bounds.withTrimmedBottom(-30)); // undo the trimming for the tab
    }

private:
    OscComp oscComp;
    EnvComp envComp;
    
    //this class does not necessarily have any need to hold these references as they are passed to the children in the constuctor.
    //I am leaving them here for convenience in case the need arises
    int operatorNum;
    juce::AudioProcessorValueTreeState& apvtsRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscEnvParent)
};


