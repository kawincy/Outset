/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OutsetAudioProcessorEditor::OutsetAudioProcessorEditor(OutsetAudioProcessor& p,
                                                        juce::MidiKeyboardState& ks)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      filter_comp(audioProcessor.apvts, audioProcessor.getRTA()),
      keyboard_comp(ks),
      alg_comp(audioProcessor.apvts),
      osc_env_tab(audioProcessor.apvts),
      header_comp(audioProcessor.getPresetManager()),
      fx_comp(audioProcessor.apvts, audioProcessor.getFXEngine())
{
    double ratio = 4.0 / 3.0;
    setResizeLimits(400, 400 / ratio, 1200, 1200 / ratio);
    getConstrainer()->setFixedAspectRatio(ratio);
    setSize(800.0, 800.0 / ratio);

    filter_comp.setSampleRate(audioProcessor.getSampleRate());

    addAndMakeVisible(header_comp);
    addAndMakeVisible(filter_comp);
    addAndMakeVisible(keyboard_comp);
    addAndMakeVisible(alg_comp);
    addAndMakeVisible(osc_env_tab);
    addAndMakeVisible(fx_comp);
    fx_comp.setVisible(fxVisible);

    header_comp.onFXButtonClicked = [this]()
    {
        fxVisible = !fxVisible;
        fx_comp.setVisible(fxVisible);
        resized();
    };

    // Wire up EQ expand button → show/hide overlay
    fx_comp.onEQExpandToggled = [this](bool isExpanded)
    {
        if (isExpanded)
            showEQOverlay();
        else
            hideEQOverlay();
    };

    setResizable(true, true);
}

OutsetAudioProcessorEditor::~OutsetAudioProcessorEditor()
{
    stopTimer();
    hideEQOverlay(); // ensure overlay is cleaned up cleanly
}

//==============================================================================
void OutsetAudioProcessorEditor::showEQOverlay()
{
    if (eqOverlay) return; // already showing

    eqOverlay = std::make_unique<EQOverlay>([this]() { hideEQOverlay(); });

    // Hand the EQ editor component to the overlay.
    // ThreeBandEQEditor stays owned by FXComp but is temporarily
    // re-parented to the overlay for display.
    eqOverlay->setEQEditor(fx_comp.getEQEditor());

    // Add on top of everything in the editor
    addAndMakeVisible(*eqOverlay);
    eqOverlay->setBounds(getLocalBounds());
    eqOverlay->toFront(false);
}

void OutsetAudioProcessorEditor::hideEQOverlay()
{
    if (!eqOverlay) return;

    // Return the EQ editor to FXComp before destroying the overlay
    if (auto* eq = fx_comp.getEQEditor())
    {
        fx_comp.addAndMakeVisible(eq); // re-parents back to FXComp
        fx_comp.resized();
    }

    removeChildComponent(eqOverlay.get());
    eqOverlay.reset();

    // Sync the button text back to "+" on the EQ editor
    // (the editor's expanded flag is still true, but the overlay is gone;
    //  clicking "+" again will re-open it cleanly)
}

//==============================================================================
void OutsetAudioProcessorEditor::paint(juce::Graphics& g)
{
}

void OutsetAudioProcessorEditor::resized()
{
    auto bounds     = getLocalBounds();
    int width_half  = getWidth() / 2;
    int height_half = getHeight() / 2;
    int height_3rd  = getHeight() / 3;
    int height_6th  = getHeight() / 6;

    header_comp.setBounds(0, 0, getWidth(), height_6th);
    osc_env_tab.setBounds(0, height_6th, width_half, 2 * height_3rd);
    alg_comp.setBounds(width_half, height_6th, width_half, height_3rd);
    filter_comp.setBounds(width_half, height_half, width_half, height_3rd);
    keyboard_comp.setBounds(0, height_6th * 5, getWidth(), height_6th);

    if (fxVisible)
        fx_comp.setBounds(0, header_comp.getBottom(), getWidth(), getHeight() - header_comp.getBottom());

    // Keep overlay covering the full window if it's open
    if (eqOverlay)
        eqOverlay->setBounds(getLocalBounds());
}