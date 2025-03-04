#include <JuceHeader.h>
#include "EnvComp.h"

//==============================================================================
EnvComp::EnvComp()
{
    initializeSlider(attackSlider, "A", 0.0, 5.0, 0.01, 0.1);
    initializeSlider(decaySlider, "D", 0.0, 5.0, 0.01, 0.1);
    initializeSlider(sustainSlider, "S", 0.0, 1.0, 0.01, 0.8);
    initializeSlider(releaseSlider, "R", 0.0, 5.0, 0.01, 0.1);
    
    // Kind of just guessed on these colors. Perhaps later we can have some
    // global color obj.
    juce::Colour mainBlue(0x91, 0xC9, 0xB5);
    juce::Colour accentBlue(0x5B, 0x8F, 0x7E);
    
    for (auto* slider : {&attackSlider, &decaySlider, &sustainSlider, &releaseSlider})
    {
        slider->setColour(juce::Slider::rotarySliderFillColourId, mainBlue);
        slider->setColour(juce::Slider::rotarySliderOutlineColourId, accentBlue);
        slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    }
}

EnvComp::~EnvComp()
{
}

void EnvComp::initializeSlider(juce::Slider& slider, const juce::String& name, double min, double max, double interval, double initialValue)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 30, 15);
    slider.setRange(min, max, interval);
    slider.setValue(initialValue);
    slider.addListener(this);

    juce::Label* label = nullptr;
    if (name == "A") label = &attackLabel;
    else if (name == "D") label = &decayLabel;
    else if (name == "S") label = &sustainLabel;
    else if (name == "R") label = &releaseLabel;

    if (label != nullptr)
    {
        addAndMakeVisible(*label);
        label->setText(name, juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centredLeft);
        label->setFont(juce::Font(14.0f, juce::Font::bold));
        label->setColour(juce::Label::textColourId, juce::Colour(0x91, 0xC9, 0xB5));
    }
}

void EnvComp::paint(juce::Graphics& g)
{
    // Off-black
    // TODO: use the colors in the notion
    g.fillAll(juce::Colour(0x1A, 0x1A, 0x1A));

    // Border
    g.setColour(juce::Colour(0x5B, 0x8F, 0x7E));
    g.drawRect(getLocalBounds(), 1);

    // Draw ADSR graph
    auto bounds = getLocalBounds().reduced(14); // padding
    auto width = bounds.getWidth();
    auto height = bounds.getHeight() * 0.7;
    auto graphBounds = bounds.withHeight(height);

    float attack = attackSlider.getValue();
    float decay = decaySlider.getValue();
    float sustain = sustainSlider.getValue();
    float release = releaseSlider.getValue();

    // Calc times
    float totalTime = attack + decay + 2.0f + release;
    float attackWidth = (attack / totalTime) * width;
    float decayWidth = (decay / totalTime) * width;
    float sustainWidth = (2.0f / totalTime) * width;
    float releaseWidth = (release / totalTime) * width;
    
    float x = graphBounds.getX();
    float y = graphBounds.getBottom();
    float peakY = graphBounds.getY();
    float sustainY = graphBounds.getY() + (1.0f - sustain) * height;

    juce::Path adsrPath;
    adsrPath.startNewSubPath(x, y);
    
    // Attack segment
    adsrPath.lineTo(x + attackWidth, peakY);
    x += attackWidth;
    // Decay segment
    adsrPath.lineTo(x + decayWidth, sustainY);
    x += decayWidth;
    // Sustain segment
    adsrPath.lineTo(x + sustainWidth, sustainY);
    x += sustainWidth;
    // Release segment
    adsrPath.lineTo(x + releaseWidth, y);

    g.setColour(juce::Colour(0x91, 0xC9, 0xB5));
    g.strokePath(adsrPath, juce::PathStrokeType(2.0f));
    
    // Gradient
    juce::Path fillPath = adsrPath;
    fillPath.lineTo(graphBounds.getRight(), graphBounds.getBottom());
    fillPath.lineTo(graphBounds.getX(), graphBounds.getBottom());
    fillPath.closeSubPath();
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(0x91, 0xC9, 0xB5).withAlpha(0.15f),
        graphBounds.getX(), graphBounds.getY(),
        juce::Colour(0x91, 0xC9, 0xB5).withAlpha(0.05f),
        graphBounds.getX(), graphBounds.getBottom(),
        false));
    g.fillPath(fillPath);
    
//    bool debug = false;
//    if (!debug) return;
//    bounds = getLocalBounds();
//    width = bounds.getWidth();
//    height = bounds.getHeight();
//    g.setColour(juce::Colours::grey);
    
    // Grid lines
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    for (int i = 1; i < 5; ++i)
    {
        float xPos = graphBounds.getX() + (graphBounds.getWidth() * i) / 5.0f;
        g.drawLine(xPos, graphBounds.getY(), xPos, graphBounds.getBottom(), 1.0f);
    }
    for (int j = 1; j < 4; ++j)
    {
        float yPos = graphBounds.getY() + (height * j) / 4.0f;
        g.drawLine(graphBounds.getX(), yPos, graphBounds.getRight(), yPos, 1.0f);
    }
    
    g.setColour(juce::Colour(0x91, 0xC9, 0xB5));
    g.setFont(juce::Font(12.0f));
    
    auto valueStr = [](float value) { return juce::String(value, 2); };
    g.drawText(valueStr(attack), attackSlider.getBounds().translated(0, -15), juce::Justification::centred);
    g.drawText(valueStr(decay), decaySlider.getBounds().translated(0, -15), juce::Justification::centred);
    g.drawText(valueStr(sustain), sustainSlider.getBounds().translated(0, -15), juce::Justification::centred);
    g.drawText(valueStr(release), releaseSlider.getBounds().translated(0, -15), juce::Justification::centred);
}

void EnvComp::resized()
{
    auto bounds = getLocalBounds();
    
    auto graphHeight = bounds.getHeight() * 0.7;
    bounds.removeFromTop(graphHeight);
    
    auto sliderArea = bounds;
    auto sliderWidth = sliderArea.getWidth() / 4;
    
    auto knobSize = juce::jmin(sliderWidth * 0.6f, 35.0f);
    
    // Lambda to position sliders and the A/D/S/R label
    auto setupSliderAndLabel = [&](juce::Slider& slider, juce::Label& label, juce::Rectangle<int> area) {
        auto sliderBounds = area.withSizeKeepingCentre(knobSize, knobSize);
        slider.setBounds(sliderBounds);
        label.setBounds(sliderBounds.withWidth(20).translated(-20, 0));
    };
    
    setupSliderAndLabel(attackSlider, attackLabel, sliderArea.removeFromLeft(sliderWidth));
    setupSliderAndLabel(decaySlider, decayLabel, sliderArea.removeFromLeft(sliderWidth));
    setupSliderAndLabel(sustainSlider, sustainLabel, sliderArea.removeFromLeft(sliderWidth));
    setupSliderAndLabel(releaseSlider, releaseLabel, sliderArea);
}

void EnvComp::sliderValueChanged(juce::Slider* slider)
{
    repaint();
}

void EnvComp::setSliderBounds(juce::Slider& slider, juce::Label& label, juce::Rectangle<int> bounds)
{
    slider.setBounds(bounds);
    label.setBounds(bounds.withWidth(20).translated(-20, 0));
}
