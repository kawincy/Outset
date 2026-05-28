/*
  ==============================================================================
    FXComp.h
    Created: 13 Nov 2025
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../FX/EffectContainer.h"
#include "../FX/OutsetVerbEngine.h"
#include "../FX/Effects/ThreeBandEQEditor.h"

//==============================================================================
class FXComp : public juce::Component,
               private juce::AudioProcessorValueTreeState::Listener
{
public:
    FXComp(juce::AudioProcessorValueTreeState& apvtsRef, OutsetVerbEngine& fxEngine);
    ~FXComp() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void(bool isExpanded)> onEQExpandToggled;
    ThreeBandEQEditor* getEQEditor() noexcept { return eqEditor.get(); }

private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    OutsetVerbEngine& fxEngine;

    // Effect containers
    std::unique_ptr<EffectContainer> bitCrusherContainer;
    std::unique_ptr<EffectContainer> delayContainer;
    std::unique_ptr<EffectContainer> reverbContainer;
    std::unique_ptr<ThreeBandEQEditor> eqEditor;

    // Per-slot bypass toggle buttons
    std::array<std::unique_ptr<juce::ToggleButton>, 4> bypassButtons;

    // Chain ordering UI
    std::array<std::unique_ptr<juce::ComboBox>, 4> chainDropdowns;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, 4> chainAttachments;
    juce::Label audioInputLabel;
    juce::Label audioOutputLabel;
    std::array<juce::Label, 3> flowArrows;
    juce::Label titleLabel;

    // Which slots are currently bypassed (not persisted — session only)
    std::array<bool, 4> slotBypassed { false, false, false, false };

    static constexpr int titleHeight         = 30;
    static constexpr int chainOrderingHeight  = 50;
    static constexpr int containerPadding     = 10;
    static constexpr int bypassButtonHeight   = 20;

    void setupEffectContainers();
    void setupChainOrderingUI();
    void setupBypassButtons();
    void updateEffectContainerStates();
    void updateChainDropdownOptions();
    void applyBypassState(int slotIndex, bool bypassed);

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FXComp)
};