/*
  ==============================================================================

    FXComp.h
    Created: 13 Nov 2025
    
    FX Rack component for Outset synthesizer.
    Provides multi-effect processing with configurable chain.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../FX/EffectContainer.h"

//==============================================================================
class FXComp : public juce::Component,
               private juce::AudioProcessorValueTreeState::Listener
{
public:
    FXComp(juce::AudioProcessorValueTreeState& apvtsRef);
    ~FXComp() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    
    // Effect containers
    std::unique_ptr<EffectContainer> bitCrusherContainer;
    std::unique_ptr<EffectContainer> delayContainer;
    std::unique_ptr<EffectContainer> eqContainer;
    std::unique_ptr<EffectContainer> reverbContainer;

    // Chain ordering UI components
    std::array<std::unique_ptr<juce::ComboBox>, 4> chainDropdowns;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, 4> chainAttachments;
    juce::Label audioInputLabel;
    juce::Label audioOutputLabel;
    std::array<juce::Label, 3> flowArrows;

    // Title label
    juce::Label titleLabel;
    
    // Layout constants
    static constexpr int titleHeight = 30;
    static constexpr int chainOrderingHeight = 50;
    static constexpr int containerPadding = 10;
    
    void setupEffectContainers();
    void setupChainOrderingUI();
    void updateEffectContainerStates();
    void updateChainDropdownOptions();

    // AudioProcessorValueTreeState::Listener override
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FXComp)
};
