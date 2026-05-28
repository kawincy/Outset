/*
  ==============================================================================

    PresetPanel.h
    Created: 16 Sep 2025 3:50:44pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PresetManager.h"

class PresetPanel : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
    {
    public:
        PresetPanel(PresetManager& pm);
        
        ~PresetPanel();
        
        void resized() override;
        
        
        
    private:
        void configureButton(juce::Button& button, const juce::String& buttonText);
        
        void buttonClicked(juce::Button* button) override;
       
        void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
        
        void loadPresetList();
        
        
        PresetManager& presetManager;
        juce::TextButton saveButton, deleteButton, previousButton, nextButton;
        juce::ComboBox presetList;
        std::unique_ptr<juce::FileChooser> fileChooser;
    };
