#include <JuceHeader.h>

#include "Colors.h"
#include "EnvComp.h"

//==============================================================================
EnvComp::EnvComp(int num, juce::AudioProcessorValueTreeState& apvtsRef)
    : envNum(num),
      apvtsRef(apvtsRef),
      attackAttachment(apvtsRef, "ATTACK_" + std::to_string(num), attackSlider),
      decayAttachment(apvtsRef, "DECAY_" + std::to_string(num), decaySlider),
      sustainAttachment(apvtsRef, "SUSTAIN_" + std::to_string(num), sustainSlider),
      releaseAttachment(apvtsRef, "RELEASE_" + std::to_string(num), releaseSlider)
 
{
    initializeSlider(attackSlider, "A", 0.0, 5.0, 0.01, 0.1, true);
    initializeSlider(decaySlider, "D", 0.0, 5.0, 0.01, 0.1, true);
    initializeSlider(sustainSlider, "S", 0.0, 1.0, 0.01, 0.8, false);
    initializeSlider(releaseSlider, "R", 0.0, 5.0, 0.01, 0.1, true);
    
    for (auto* slider : {&attackSlider, &decaySlider, &sustainSlider, &releaseSlider})
    {
        slider->setColour(juce::Slider::rotarySliderFillColourId, colors().main);
        slider->setColour(juce::Slider::rotarySliderOutlineColourId, colors().accent);
        slider->setColour(juce::Slider::thumbColourId, colors().white);
    }
}

EnvComp::~EnvComp()
{
}

void EnvComp::initializeSlider(juce::Slider& slider, const juce::String& name, double min, double max, double interval, double initialValue, bool skewed)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 30, 15);
    slider.setRange(min, max, interval);
    slider.setValue(initialValue);
    slider.addListener(this);
    if (skewed) slider.setSkewFactorFromMidPoint(1.0); // skews a, d, and r to have 1 second be the midpoint

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
        label->setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
        label->setColour(juce::Label::textColourId, colors().main);
    }
}

void EnvComp::paint(juce::Graphics& g)
{
    g.fillAll(colors().bg);

    g.setColour(colors().accent);
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
    
    adsrPath.lineTo(x + attackWidth, peakY);
    x += attackWidth;
    adsrPath.lineTo(x + decayWidth, sustainY);
    x += decayWidth;
    adsrPath.lineTo(x + sustainWidth, sustainY);
    x += sustainWidth;
    adsrPath.lineTo(x + releaseWidth, y);

    g.setColour(colors().main);
    g.strokePath(adsrPath, juce::PathStrokeType(2.0f));
    
    // Gradient
    juce::Path fillPath = adsrPath;
    fillPath.lineTo(graphBounds.getRight(), graphBounds.getBottom());
    fillPath.lineTo(graphBounds.getX(), graphBounds.getBottom());
    fillPath.closeSubPath();
    g.setGradientFill(juce::ColourGradient(
        colors().main.withAlpha(0.15f),
        graphBounds.getX(), graphBounds.getY(),
        colors().main.withAlpha(0.05f),
        graphBounds.getX(), graphBounds.getBottom(),
        false));
    g.fillPath(fillPath);
    
    g.setColour(colors().gridlines);
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
    
    g.setColour(colors().main);
    g.setFont(juce::Font(juce::FontOptions(12.0f)));
    
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
