/*
  ==============================================================================

    OutsetVerbUI.h
    
    User interface component for Outset-Verb multi-effect processor.
    This class encapsulates all UI logic and can be used independently
    of JUCE's AudioProcessorEditor framework.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EffectContainer.h"
#include <memory>

//==============================================================================
/**
    User interface component for Outset-Verb effects.
    
    This class manages all UI components without being tied to the
    AudioProcessorEditor framework, making it reusable in other contexts.
*/
class OutsetVerbUI : public juce::Component,
                     private juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    /** Constructor - accepts reference to external APVTS for parameter management. */
    OutsetVerbUI(juce::AudioProcessorValueTreeState& apvtsRef);
    
    /** Destructor. */
    ~OutsetVerbUI() override;
    
    //==============================================================================
    /** Component overrides. */
    void paint(juce::Graphics&) override;
    void resized() override;
    
    //==============================================================================
    /** Returns the recommended size for this UI component. */
    static juce::Rectangle<int> getRecommendedSize();
    
private:
    //==============================================================================
    // Reference to external APVTS (not owned by this class)
    juce::AudioProcessorValueTreeState& apvts;
    
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

    // Main title label
    juce::Label titleLabel;
    
    // Layout constants
    static constexpr int windowWidth = 950;
    static constexpr int windowHeight = 700;
    static constexpr int titleHeight = 40;
    static constexpr int chainOrderingHeight = 60;
    static constexpr int containerPadding = 12;
    
    //==============================================================================
    /** Initializes all effect containers with their parameters. */
    void setupEffectContainers();

    /** Initializes the chain ordering UI components. */
    void setupChainOrderingUI();

    /** Updates EffectContainer enabled states based on current chain configuration. */
    void updateEffectContainerStates();

    /** Updates the available options in chain dropdowns based on current selections. */
    void updateChainDropdownOptions();

    // AudioProcessorValueTreeState::Listener override
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutsetVerbUI)
};
