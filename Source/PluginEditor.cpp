/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OutsetAudioProcessorEditor::OutsetAudioProcessorEditor (OutsetAudioProcessor& p, juce::MidiKeyboardState& ks )
: AudioProcessorEditor (&p), audioProcessor (p), filter_comp(audioProcessor.apvts, audioProcessor.getRTA()), keyboard_comp(ks), alg_comp(audioProcessor.apvts), osc_env_tab(audioProcessor.apvts),
    header_comp(audioProcessor.getPresetManager()), fx_comp(audioProcessor.apvts)
{
    double ratio = 4.0 / 3.0;
    setResizeLimits(400, 400 / ratio, 1200, 1200 / ratio);
    getConstrainer()->setFixedAspectRatio(ratio);
    setSize(800.0, 800.0 / ratio);
    // Sync filter graph sample rate to processor
    filter_comp.setSampleRate(audioProcessor.getSampleRate());

    addAndMakeVisible(header_comp);
    //addAndMakeVisible(env_comp);
    addAndMakeVisible(filter_comp);
    addAndMakeVisible(keyboard_comp);
    addAndMakeVisible(alg_comp);
    addAndMakeVisible(osc_env_tab);
    addAndMakeVisible(fx_comp);
    fx_comp.setVisible(fxVisible);
    
    // Setup FX button callback
    header_comp.onFXButtonClicked = [this]() {
        fxVisible = !fxVisible;
        fx_comp.setVisible(fxVisible);
        resized();
    };
    
	setResizable(true, true);
    
}

OutsetAudioProcessorEditor::~OutsetAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void OutsetAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Probably will get rid of this because it'll get covered by the components
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Outset Island", getLocalBounds(), juce::Justification::centred, 1);
}

void OutsetAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    int width_half = getWidth() / 2;
    int height_half = getHeight() / 2;
    int height_3rd = getHeight() / 3;
    int height_6th = getHeight() / 6;
    // Header at top
    header_comp.setBounds(0, 0, getWidth(), height_6th);
    osc_env_tab.setBounds(0, height_6th, width_half, 2 * height_3rd);
    //env_comp.setBounds(0, height_half, width_half, height_3rd);

    // Bottom Half
    alg_comp.setBounds(width_half, height_6th, width_half, height_3rd);
    filter_comp.setBounds(width_half, height_half, width_half, height_3rd);
    keyboard_comp.setBounds(0, height_6th * 5, getWidth(), height_6th);

    // FX overlay - covers everything below header when visible
    if (fxVisible)
    {
        fx_comp.setBounds(0, header_comp.getBottom(), getWidth(), getHeight() - header_comp.getBottom());
    }
}
