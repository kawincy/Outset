/*
  ==============================================================================

    PresetManager.h
    Created: 16 Sep 2025 4:00:57pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PresetManager : juce::ValueTree::Listener
{
public:
    static const juce::File defaultDirectory;
    static const juce::String extension;
    static const juce::String presetNameProperty;
    
    PresetManager(juce::AudioProcessorValueTreeState& );
    
    void savePreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    int loadNextPreset();
    int loadPreviousPreset();
    juce::StringArray getAllPresets() const;
    juce::String getCurrentPreset() const;
    
private:
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;
    
    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::Value currentPreset;
    
};
