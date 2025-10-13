/*
  ==============================================================================

    PresetPanel.cpp
    Created: 16 Sep 2025 3:50:44pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#include "PresetPanel.h"
#include "GUI/Colors.h"

PresetPanel::PresetPanel(PresetManager& pm) : presetManager(pm)
{
    configureButton(saveButton, "Save");
    configureButton(deleteButton, "Delete");
    configureButton(previousButton, "<-");
    configureButton(nextButton, "->");
    
    presetList.setTextWhenNothingSelected("None");
    presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    presetList.setColour(juce::ComboBox::backgroundColourId, colors().bg);

    addAndMakeVisible(presetList);
    presetList.addListener(this);
    
    
    loadPresetList();
}


PresetPanel::~PresetPanel()
{
    saveButton.removeListener(this);
    deleteButton.removeListener(this);
    previousButton.removeListener(this);
    nextButton.removeListener(this);
    presetList.removeListener(this);
}

void PresetPanel::resized()
{
    const auto container = getLocalBounds().reduced(4);
    auto bounds = container;
    
    saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
    previousButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
    presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)).reduced(4));
    nextButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
    deleteButton.setBounds(bounds.reduced(4));
}

void PresetPanel::configureButton(juce::Button& button, const juce::String& buttonText)
{
    button.setButtonText(buttonText);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    button.setColour(juce::TextButton::buttonColourId, colors().bg);
    //need custom look and feel to change border color
    
    
    addAndMakeVisible(button);
    button.addListener(this);
}

void PresetPanel::buttonClicked(juce::Button* button) {
    
    if (button == &saveButton) {
        fileChooser = std::make_unique<juce::FileChooser>("please enter the name of the preset to save",
                            PresetManager::defaultDirectory, "*." + PresetManager::extension);
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser) {
            const auto resultFile = chooser.getResult();
            presetManager.savePreset(resultFile.getFileNameWithoutExtension());
            loadPresetList();
        });
    }
    
    if (button == &previousButton)
    {
        const int index = presetManager.loadPreviousPreset();
        presetList.setSelectedItemIndex(index, juce::dontSendNotification);
    }
    if (button == &nextButton)
    {
        const int index = presetManager.loadNextPreset();
        presetList.setSelectedItemIndex(index, juce::dontSendNotification);
    }
    if (button == &deleteButton)
    {
        presetManager.deletePreset(presetManager.getCurrentPreset());
        loadPresetList();
    }
}

void PresetPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
}

void PresetPanel::loadPresetList() {
    presetList.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getAllPresets();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetList.addItemList(allPresets, 1);
    presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
}
