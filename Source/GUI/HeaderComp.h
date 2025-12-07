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
    
    // Callback for FX button
    std::function<void()> onFXButtonClicked;

private:
    PresetPanel presetPanel;
    juce::TextButton fxButton;
    
    void showFXPopup();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComp)
};
