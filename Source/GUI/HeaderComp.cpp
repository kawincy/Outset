/*
  ==============================================================================

    HeaderComp.cpp
    Created: 2 Feb 2025 12:37:21pm
    Author:  josep

  ==============================================================================
*/

#include <JuceHeader.h>

#include "Colors.h"
#include "HeaderComp.h"

//==============================================================================
HeaderComp::HeaderComp(PresetManager& pm) :
    presetPanel(pm)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(presetPanel);
    
    // Setup FX button
    fxButton.setButtonText("FX");
    fxButton.setColour(juce::TextButton::buttonColourId, colors().bg);
    fxButton.setColour(juce::TextButton::buttonOnColourId, colors().accent);
    fxButton.setColour(juce::TextButton::textColourOffId, colors().white);
    fxButton.setColour(juce::TextButton::textColourOnId, colors().white);
    fxButton.onClick = [this] { showFXPopup(); };
    addAndMakeVisible(fxButton);
}

HeaderComp::~HeaderComp()
{
}

void HeaderComp::paint (juce::Graphics& g)
{
    g.fillAll(colors().bg);

    //g.setColour(colors().main);
    //g.setFont(juce::FontOptions(14.0f));
    //g.drawText("Presets", getLocalBounds(), juce::Justification::centred, true);
}

void HeaderComp::resized()
{
    auto bounds = getBounds();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();

    presetPanel.setBounds(getBounds().withTrimmedRight(width/3));
    
    // Position FX button in the remaining 1/3 with 10px margins
    auto fxBounds = bounds.removeFromRight(width/3).reduced(10);
    fxButton.setBounds(fxBounds);
}

void HeaderComp::showFXPopup()
{
    // Placeholder for FX container
    class FXRackPopup : public juce::Component
    {
    public:
        FXRackPopup()
        {
            setSize(400, 300);
        }
        
        void paint(juce::Graphics& g) override
        {
            g.fillAll(colors().bg);
            g.setColour(colors().accent);
            g.drawRect(getLocalBounds(), 2);
            
            g.setColour(colors().white);
            g.setFont(juce::FontOptions(16.0f));
            g.drawText("FX Rack (Coming Soon)", getLocalBounds(), 
                      juce::Justification::centred, true);
        }
    };
    
    auto* popup = new FXRackPopup();
    auto& callout = juce::CallOutBox::launchAsynchronously(
        std::unique_ptr<juce::Component>(popup),
        fxButton.getScreenBounds(),
        nullptr
    );
    callout.setDismissalMouseClicksAreAlwaysConsumed(true);
}
