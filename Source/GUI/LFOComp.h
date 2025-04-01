/*
  ==============================================================================

    LFOComp.h
    Created: 2 Feb 2025 12:49:27pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include "AlgoBoxComp.h"
#include <JuceHeader.h>

//==============================================================================
/*
*/
class LFOComp  : public juce::Component
{
public:
    LFOComp();
    ~LFOComp() override;

    void paint (juce::Graphics&) override;
    void resized() override;


private:
    int algo_ind;

    AlgoBoxComp test;
    AlgoBoxComp test2;

    AlgoBoxComp grid[4][6];
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOComp)
};
