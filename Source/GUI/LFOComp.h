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
    LFOComp(juce::AudioProcessorValueTreeState& apvtsRef);
    ~LFOComp() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setAlgIndexParameter(int newValue);


private:
    int algo_ind;
    juce::AudioProcessorValueTreeState& apvtsRef;

    std::unique_ptr<juce::DrawableButton> next_b;
    std::unique_ptr<juce::DrawableButton> prev_b;

    AlgoBoxComp test;
    AlgoBoxComp test2;

    // used for grid drawings
    AlgoBoxComp grid[4][6];
    int rows, cols, box_width, box_height, spacing_x, spacing_y;
    void LFOComp::draw_boxes();
    void LFOComp::undraw_boxes();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOComp)
};
    