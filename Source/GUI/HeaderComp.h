/*
  ==============================================================================

    HeaderComp.h
    Created: 2 Feb 2025 12:37:21pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetPanel.h"

//==============================================================================
/*
*/
class HeaderComp  : public juce::Component
{
public:
    HeaderComp(PresetManager&);
    ~HeaderComp() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PresetPanel presetPanel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComp)
};
