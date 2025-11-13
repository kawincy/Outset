/*
  ==============================================================================

    EffectContainer.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>

//==============================================================================
/**
    A container component that groups related effect parameters with a title.
    
    This component provides a clean way to organize sliders and other controls
    for a specific audio effect, with automatic layout management.
*/
class EffectContainer : public juce::Component
{
public:
    //==============================================================================
    enum class LayoutMode { Auto, SingleColumn, TwoColumn };

    EffectContainer(const juce::String& title, LayoutMode mode = LayoutMode::Auto);
    ~EffectContainer() override = default;

    //==============================================================================
    /** Adds a slider with automatic attachment to APVTS parameter. */
    void addSlider(const juce::String& parameterID,
                   const juce::String& labelText,
                   juce::AudioProcessorValueTreeState& apvts);

    /** Adds a toggle button with automatic attachment to APVTS parameter. */
    void addToggleButton(const juce::String& parameterID,
                        const juce::String& labelText,
                        juce::AudioProcessorValueTreeState& apvts);

    /** Sets the enabled state of the container (affects visual appearance). */
    void setEnabledState(bool enabled);

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    juce::String containerTitle;
    juce::Label titleLabel;
    
    // Container for all parameter controls
    struct ParameterControl
    {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::ToggleButton> toggleButton;
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
        
        ParameterControl() = default;
        ~ParameterControl() = default;
        
        // Make it movable but not copyable
        ParameterControl(const ParameterControl&) = delete;
        ParameterControl& operator=(const ParameterControl&) = delete;
        ParameterControl(ParameterControl&&) = default;
        ParameterControl& operator=(ParameterControl&&) = default;
    };
    
    std::vector<ParameterControl> controls;

    // Layout mode and constants
    LayoutMode layoutMode;
    bool isEnabled = true;  // Track enabled state for visual feedback
    static constexpr int titleHeight = 25;
    static constexpr int controlHeight = 60;
    static constexpr int padding = 8;
    static constexpr int labelHeight = 20;
    static constexpr int columnSpacing = 4;
    static constexpr int minControlsForTwoColumn = 6;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectContainer)
};
