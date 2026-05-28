/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/HeaderComp.h"
#include "GUI/EnvComp.h"
#include "GUI/FilterComp.h"
#include "GUI/KeyboardComp.h"
#include "GUI/AlgComp.h"
#include "GUI/OscComp.h"
#include "GUI/OscEnvTab.h"
#include "GUI/Scope.h"
#include "GUI/FXComp.h"
#include "FX/Effects/ThreeBandEQEditor.h"

//==============================================================================
class EQOverlay : public juce::Component
{
public:
    explicit EQOverlay(std::function<void()> closeCallback)
        : onClose(std::move(closeCallback))
    {
        setInterceptsMouseClicks(true, true);

        closeButton.setButtonText("Close EQ");
        closeButton.setColour(juce::TextButton::buttonColourId,  juce::Colour(0xff2a2a3e));
        closeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff00c8ff));
        closeButton.onClick = [this]() { if (onClose) onClose(); };
        addAndMakeVisible(closeButton);
    }

    void setEQEditor(ThreeBandEQEditor* editor)
    {
        if (eqEditor) removeChildComponent(eqEditor);
        eqEditor = editor;
        if (eqEditor) { addAndMakeVisible(eqEditor); resized(); }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xcc000000));
        g.setColour(juce::Colour(0xff1a1a2e));
        g.fillRoundedRectangle(panelBounds.toFloat(), 8.0f);
        g.setColour(colors().accent);
        g.drawRoundedRectangle(panelBounds.toFloat().reduced(1.0f), 8.0f, 1.5f);
    }

    void resized() override
    {
        const int pw = juce::roundToInt(getWidth()  * 0.85f);
        const int ph = juce::roundToInt(getHeight() * 0.85f);
        panelBounds = juce::Rectangle<int>((getWidth() - pw) / 2, (getHeight() - ph) / 2, pw, ph);

        const int btnW = 90, btnH = 26, margin = 10;
        closeButton.setBounds(panelBounds.getRight() - btnW - margin,
                              panelBounds.getY() + margin, btnW, btnH);

        if (eqEditor)
        {
            auto eqArea = panelBounds.reduced(margin);
            eqArea.removeFromTop(btnH + margin);
            eqEditor->setBounds(eqArea);
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        if (!panelBounds.contains(e.getPosition()))
            if (onClose) onClose();
    }

private:
    std::function<void()> onClose;
    ThreeBandEQEditor*    eqEditor { nullptr };
    juce::TextButton      closeButton;
    juce::Rectangle<int>  panelBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQOverlay)
};

//==============================================================================
class OutsetAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    OutsetAudioProcessorEditor(OutsetAudioProcessor&, juce::MidiKeyboardState& ks);
    ~OutsetAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OutsetAudioProcessor& audioProcessor;
    HeaderComp   header_comp;
    FilterComp   filter_comp;
    KeyboardComp keyboard_comp;
    AlgComp      alg_comp;
    OscEnvTab    osc_env_tab;
    FXComp       fx_comp;

    std::unique_ptr<EQOverlay> eqOverlay;
    bool fxVisible = false;

    void showEQOverlay();
    void hideEQOverlay();
    void repaintEverything();

    void timerCallback() override {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutsetAudioProcessorEditor)
};