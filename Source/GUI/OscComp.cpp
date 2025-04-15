/*
  ==============================================================================

    OscComp.cpp
    Created: 2 Feb 2025 12:46:52pm
    Author:  josep, ryanb

  ==============================================================================
*/
#include <string>

#include "Colors.h"
#include "OscComp.h"

OscComp::OscComp(int num, juce::AudioProcessorValueTreeState& apvtsRef)
    : oscNum(num),
      apvtsRef(apvtsRef),
      levelAttachment(apvtsRef, "LEVEL_" + std::to_string(oscNum), oscLevelSlider),
      fineAttachment(apvtsRef, "FINE_" + std::to_string(oscNum), oscFineSlider),
      coarseAttachment(apvtsRef, "COARSE_" + std::to_string(oscNum), oscCoarseSlider),
	  ratioAttachment(apvtsRef, "RATIO_" + std::to_string(oscNum), oscRatioSlider)
      
{
	
    // setup the sliders
    initializeSlider(oscLevelSlider, oscLevelLabel, "Level", juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, 0.5);
    initializeSlider(oscFineSlider, oscFineLabel, "Fine", juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, 0.0);
    initializeSlider(oscCoarseSlider, oscCoarseLabel, "Coarse", juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, 0.0);
    initializeSlider(oscRatioSlider, oscRatioLabel, "Ratio", juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, 1.0, 1.0);

    for (auto* slider : {&oscLevelSlider, &oscFineSlider, &oscCoarseSlider, &oscRatioSlider})
    {
        slider->setColour(juce::Slider::rotarySliderFillColourId, colors().main);
        slider->setColour(juce::Slider::rotarySliderOutlineColourId, colors().accent);
        slider->setColour(juce::Slider::thumbColourId, colors().white);
    }
    
    for (auto* label : {&oscLevelLabel, &oscFineLabel, &oscCoarseLabel, &oscRatioLabel})
    {
        label->setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
        label->setColour(juce::Label::textColourId, colors().main);
        label->setJustificationType(juce::Justification::centred);
    }
    oscFineSlider.setNumDecimalPlacesToDisplay(0);
    oscCoarseSlider.setNumDecimalPlacesToDisplay(0);
}

void OscComp::paint(juce::Graphics& g)
{
    g.fillAll(colors().bg);
    
    g.setColour(colors().accent);
    // g.drawRect(getLocalBounds(), 1);
    
    // DRAW OSC
    auto bounds = getLocalBounds().reduced(14); // padding
    auto height = bounds.getHeight() * 0.5; // half the height
    auto graphBounds = bounds.withHeight(height);
    
    
    juce::Path wavePath;
    float amplitude = height / 2.0f * oscLevelSlider.getValue();
    float centerY = graphBounds.getCentreY();
    float ratio = oscRatioSlider.getValue();
    float fineTune = oscFineSlider.getValue() / 25.0f;
    
    // The thing about fractional ratios
    ratio = std::max(1.0f, std::abs(ratio));
    
    wavePath.startNewSubPath(graphBounds.getX(), centerY);
    
    for (int x = 0; x < graphBounds.getWidth(); ++x)
    {
        float t = (float)x / graphBounds.getWidth() * juce::MathConstants<float>::twoPi * ratio;
        t += fineTune;
        float y = centerY - std::sin(t) * amplitude;
        wavePath.lineTo(graphBounds.getX() + x, y);
    }
    
    g.setColour(colors().main);
    g.strokePath(wavePath, juce::PathStrokeType(2.0f));
    
    // classic gradient
    juce::Path fillPath = wavePath;
    fillPath.lineTo(graphBounds.getRight(), centerY);
    fillPath.lineTo(graphBounds.getX(), centerY);
    fillPath.closeSubPath();
    g.setGradientFill(juce::ColourGradient(
        colors().main.withAlpha(0.15f),
        graphBounds.getX(), graphBounds.getY(),
        colors().main.withAlpha(0.05f),
        graphBounds.getX(), graphBounds.getBottom(),
        false));
    g.fillPath(fillPath);
    
    // grid lines
    g.setColour(colors().gridlines);
    for (int i = 1; i < 5; ++i)
    {
        float xPos = graphBounds.getX() + (graphBounds.getWidth() * i) / 5.0f;
        g.drawLine(xPos, graphBounds.getY(), xPos, graphBounds.getBottom(), 1.0f);
    }
    
    // middle line
    float centerLine = graphBounds.getCentreY();
    g.drawLine(graphBounds.getX(), centerLine, graphBounds.getRight(), centerLine, 1.0f);
    
    // vals on sliders same as ENV
    g.setColour(colors().main);
    g.setFont(juce::Font(juce::FontOptions(12.0f)));
    // lambda for printing the vals
    auto valueStr = [](float value) { return juce::String(value, 2); };
    g.drawText(valueStr(oscLevelSlider.getValue()),
              oscLevelSlider.getBounds().translated(0, -15),
              juce::Justification::centred);
    g.drawText(valueStr(oscFineSlider.getValue()),
              oscFineSlider.getBounds().translated(0, -15),
              juce::Justification::centred);
    g.drawText(valueStr(oscCoarseSlider.getValue()),
              oscCoarseSlider.getBounds().translated(0, -15),
              juce::Justification::centred);
    g.drawText(valueStr(oscRatioSlider.getValue()),
              oscRatioSlider.getBounds().translated(0, -15),
              juce::Justification::centred);
}

void OscComp::resized()
{
    auto bounds = getLocalBounds();
    
    // half height is wavepath
    auto visualHeight = bounds.getHeight() * 0.5;
    bounds.removeFromTop(visualHeight);
    
    // bottom half is sliders
    auto sliderArea = bounds;
    auto sliderWidth = sliderArea.getWidth() / 4;
    
    auto knobSize = juce::jmin(sliderWidth * 0.6f, 35.0f);
    
    // align
    auto levelArea = sliderArea.removeFromLeft(sliderWidth);
    auto fineArea = sliderArea.removeFromLeft(sliderWidth);
	auto coarseArea = sliderArea.removeFromLeft(sliderWidth);
    auto ratioArea = sliderArea;
    
    oscLevelSlider.setBounds(levelArea.withSizeKeepingCentre(knobSize, knobSize));
    oscFineSlider.setBounds(fineArea.withSizeKeepingCentre(knobSize, knobSize));
    oscCoarseSlider.setBounds(coarseArea.withSizeKeepingCentre(knobSize, knobSize));
    oscRatioSlider.setBounds(ratioArea.withSizeKeepingCentre(knobSize, knobSize));
    
    // labels below sliders
    oscLevelLabel.setBounds(oscLevelSlider.getBounds().withHeight(20).translated(0, knobSize/2 + 5));
    oscFineLabel.setBounds(oscFineSlider.getBounds().withHeight(20).translated(0, knobSize/2 + 5));
    oscCoarseLabel.setBounds(oscCoarseSlider.getBounds().withHeight(20).translated(0, knobSize/2 + 5));
    oscRatioLabel.setBounds(oscRatioSlider.getBounds().withHeight(20).translated(0, knobSize / 2 + 5));

}

void OscComp::initializeSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, juce::Slider::SliderStyle style, double initialValue)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    //slider.setRange(min, max, interval);
    //slider.setValue(initialValue);
    slider.addListener(this);
    slider.setDoubleClickReturnValue(true, initialValue);

    addAndMakeVisible(label);
    label.setText(name, juce::dontSendNotification);
}
void OscComp::initializeSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, juce::Slider::SliderStyle style, double initialValue, double midpointValue)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    //slider.setRange(min, max, interval);
    //slider.setValue(initialValue);
    slider.addListener(this);
    slider.setDoubleClickReturnValue(true, initialValue);
    slider.setSkewFactorFromMidPoint(midpointValue);

    addAndMakeVisible(label);
    label.setText(name, juce::dontSendNotification);
}

void OscComp::sliderValueChanged(juce::Slider* slider)
{
    repaint();
}
