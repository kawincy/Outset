/*
  ==============================================================================
    HeaderComp.h
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetPanel.h"
#include "ThemePicker.h"

//==============================================================================
class HeaderComp : public juce::Component
{
public:
    HeaderComp(PresetManager&);
    ~HeaderComp() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void()> onFXButtonClicked;
    std::function<void()> onThemeChanged;

private:
    PresetPanel      presetPanel;
    juce::TextButton fxButton;
    juce::TextButton themeButton;  // small toggle button

    // Floating popup — created/destroyed on demand
    std::unique_ptr<juce::Component> themePopup;

    void showThemePopup();
    void hideThemePopup();
    void showFXPopup();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderComp)
};