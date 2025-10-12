/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/PresetsComp.h"
#include "GUI/EnvComp.h"
#include "GUI/FilterComp.h"
#include "GUI/KeyboardComp.h"
#include "GUI/AlgComp.h"
#include "GUI/OscComp.h"
#include "GUI/OscEnvTab.h"
#include "GUI/Scope.h"
//==============================================================================
/**
*/
class OutsetAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    OutsetAudioProcessorEditor (OutsetAudioProcessor&, juce::MidiKeyboardState& ks /*this gets passed to keyboard_comp*/);
    ~OutsetAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OutsetAudioProcessor& audioProcessor;
    PresetsComp presets_comp;
    //EnvComp env_comp;
    FilterComp filter_comp;
    KeyboardComp keyboard_comp;
    AlgComp alg_comp;
    //OscComp osc_comp;
	OscEnvTab osc_env_tab;

    void timerCallback() override
    {
        //scopeComponent.setAudioData(audioProcessor.getAudioBuffer());
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutsetAudioProcessorEditor)
};
