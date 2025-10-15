/*
  ==============================================================================

    FilterComp.cpp
    Created: 2 Feb 2025 12:46:44pm
    Author:  josep

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterComp.h"
#include "Colors.h"

FilterComp::FilterComp(juce::AudioProcessorValueTreeState& apvtsRef) : apvtsRef(apvtsRef),
cutoffAttachment(apvtsRef, "CUTOFF", cutoffSlider),
resonanceAttachment(apvtsRef, "RESONANCE", resonanceSlider)
{
    for (auto* slider : { &cutoffSlider, &resonanceSlider })
    {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 10);

        slider->setColour(juce::Slider::rotarySliderFillColourId, colors().main);
        slider->setColour(juce::Slider::rotarySliderOutlineColourId, colors().accent);
        slider->setColour(juce::Slider::thumbColourId, colors().white);
        slider->addListener(this);
    }

    // Configure cutoff frequency knob (20 Hz to 20 kHz, logarithmic behavior)
    cutoffSlider.setRange(20.0, 20000.0, 1.0);
    cutoffSlider.setSkewFactorFromMidPoint(1000.0);
    cutoffSlider.setValue(20000.0);
    addAndMakeVisible(cutoffSlider);

    // Configure resonance (Q) knob (range from 0.1 to 10, default 0.707 for Butterworth)
    resonanceSlider.setRange(0.1, 10.0, 0.1);
    resonanceSlider.setValue(0.707);
    addAndMakeVisible(resonanceSlider);


    freqLabel.setText("Freq", juce::dontSendNotification);
    qLabel.setText("Q", juce::dontSendNotification);
    for (auto* label : { &freqLabel, &qLabel }) {
        label->setJustificationType(juce::Justification::centredLeft);
        label->setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
        label->setColour(juce::Label::textColourId, colors().main);

    }
    addAndMakeVisible(freqLabel);
    addAndMakeVisible(qLabel);


    //for (auto* textbox : { &cutoffTextBox, &resonanceTextBox }) {
        cutoffTextBox.setMultiLine(false);
        cutoffTextBox.setReturnKeyStartsNewLine(false);
        cutoffTextBox.setText(juce::String(cutoffSlider.getValue()), false);
        cutoffTextBox.setJustification(juce::Justification::centredTop);
        cutoffTextBox.applyFontToAllText(juce::Font(juce::FontOptions(12.0f, juce::Font::plain)));
        cutoffTextBox.applyColourToAllText(colors().main);
        resonanceTextBox.setMultiLine(false);
        resonanceTextBox.setReturnKeyStartsNewLine(false);
        resonanceTextBox.setText(juce::String(resonanceSlider.getValue()), false);
        resonanceTextBox.setJustification(juce::Justification::centredTop);
        resonanceTextBox.applyFontToAllText(juce::Font(juce::FontOptions(12.0f, juce::Font::plain)));
        resonanceTextBox.applyColourToAllText(colors().main);
    //}

    // Cutoff text box setup
    cutoffTextBox.onReturnKey = [this] {
        double value = cutoffTextBox.getText().getDoubleValue();
        cutoffSlider.setValue(value);
    };
    cutoffTextBox.onFocusLost = [this] {
        double value = cutoffTextBox.getText().getDoubleValue();
        cutoffSlider.setValue(value);
    };

    addAndMakeVisible(cutoffTextBox);

    // Resonance text box setup
    resonanceTextBox.onReturnKey = [this] {
        double value = resonanceTextBox.getText().getDoubleValue();
        resonanceSlider.setValue(value);
    };
    resonanceTextBox.onFocusLost = [this] {
        double value = resonanceTextBox.getText().getDoubleValue();
        resonanceSlider.setValue(value);
    };
    addAndMakeVisible(resonanceTextBox);

}

FilterComp::~FilterComp()
{
}

void FilterComp::paint(juce::Graphics& g)
{
    g.fillAll(colors().bg);

    g.setColour(colors().accent);
    g.drawRect(getLocalBounds(), 1);
    // Define drawing area for the frequency response (with margin)
    //auto drawArea = getLocalBounds().reduced(10);

    auto bounds = getLocalBounds().reduced(14); // padding
    auto height = bounds.getHeight() * 0.7;
    graphBounds = bounds.withHeight(height); // Store for mouse interaction
    auto drawArea = graphBounds;


    g.setColour(juce::Colours::white.withAlpha(0.2f));
    for (int i = 1; i < 5; ++i)
    {
        float xPos = drawArea.getX() + (drawArea.getWidth() * i) / 5.0f;
        g.drawLine(xPos, drawArea.getY(), xPos, drawArea.getBottom(), 1.0f);
    }
    for (int j = 1; j < 4; ++j)
    {
        float yPos = drawArea.getY() + (height * j) / 4.0f;
        g.drawLine(drawArea.getX(), yPos, drawArea.getRight(), yPos, 1.0f);
    }
    // -=======================================-

    // Draw horizontal grid lines (for reference)
    g.setColour(colors().gridlines);
    bool debug = false;
    if(debug)
    {
        for (int i = 0; i < 5; ++i)
        {
            float y = juce::jmap((float)i, 0.0f, 4.0f,
                (float)drawArea.getY(),
                (float)drawArea.getBottom());
            g.drawHorizontalLine(y, (float)drawArea.getX(), (float)drawArea.getRight());
        }
    }



    // Retrieve current cutoff and resonance from sliders
    float cutoff = (float)cutoffSlider.getValue();
    float Q = (float)resonanceSlider.getValue();

    // Compute TPT filter parameters
    // According to the TPT formulation:
    //   g = tan(pi * cutoff / sampleRate)
    //   R2 = 1 / Q
    //   D = 1 + R2 * g + g * g
    //   a0 = g^2 / D, a1 = 2*a0, a2 = a0
    //   b1 = -2*(1 - g^2)/D, b2 = (1 - R2*g + g*g)/D
    float gVal = std::tan(juce::MathConstants<float>::pi * cutoff / sampleRate);
    float R2 = 1.0f / Q;
    float D = 1.0f + R2 * gVal + gVal * gVal;
    float a0 = (gVal * gVal) / D;
    float a1 = 2.0f * a0;
    float a2 = a0;
    float b1 = -2.0f * (1.0f - gVal * gVal) / D;
    float b2 = (1.0f - R2 * gVal + gVal * gVal) / D;

    // Frequency range for drawing: from 20 Hz to Nyquist
    float minFreq = 20.0f;
    float maxFreq = sampleRate * 0.5f;

    juce::Path responseCurve;
    const int numPoints = drawArea.getWidth();

    // Define dB scale (0 dB at top, -60 dB at bottom)
    float maxDecibels = 20.0f;
    float minDecibels = -40.0f;

    bool firstPoint = true;
    bool inRange = false;
    float lastX = 0.0f, lastY = 0.0f;

    // Loop across the width and compute magnitude response at each frequency
    for (int i = 0; i < numPoints; ++i)
    {
        // Map x coordinate to frequency (logarithmically)
        float normX = (float)i / (float)(numPoints - 1);
        float freq = minFreq * std::pow(maxFreq / minFreq, normX);
        float omega = 2.0f * juce::MathConstants<float>::pi * freq / sampleRate;

        // Compute the complex frequency response H(e^(jω)) for the low-pass output:
        // H(e^(jω)) = (a0 + a1*e^(-jω) + a2*e^(-j2ω)) / (1 + b1*e^(-jω) + b2*e^(-j2ω))
        std::complex<float> expNegjOmega = std::exp(std::complex<float>(0, -omega));
        std::complex<float> expNegj2Omega = std::exp(std::complex<float>(0, -2 * omega));
        std::complex<float> numerator = a0 + a1 * expNegjOmega + a2 * expNegj2Omega;
        std::complex<float> denominator = std::complex<float>(1, 0) + b1 * expNegjOmega + b2 * expNegj2Omega;
        float magnitude = std::abs(numerator / denominator);

        // Convert magnitude to decibels
        float dB = juce::Decibels::gainToDecibels(magnitude);
        if (dB < minDecibels)
        {
            // If we were "inRange" on the previous sample, we can optionally
            // draw a line to the boundary:
            if (inRange)
            {
                // Compute intersection at dB = minDecibels:
                // Optionally do a little linear interpolation in the frequency domain
                // Or just lineTo the last X, Y clamped to minDbs. Example:
                float clippedY = juce::jmap(minDecibels,
                    maxDecibels, minDecibels,
                    (float)drawArea.getY(), (float)drawArea.getBottom());

                responseCurve.lineTo(lastX, clippedY);
            }
            inRange = false;
            // Do NOT do a lineTo, so we effectively skip drawing below minDbs.
        }
        else
        {
            // We are above minDecibels
            float clampedDb = juce::jlimit(minDecibels, maxDecibels, dB);
            float y = juce::jmap(clampedDb,
                maxDecibels, minDecibels,
                (float)drawArea.getY(), (float)drawArea.getBottom());
            float x = drawArea.getX() + (float)i;

            if (!inRange)
            {
                // Just transitioned from out-of-range to in-range,
                // so start a new subpath here
                responseCurve.startNewSubPath(x, y);
                inRange = true;
            }
            else
            {
                // Continue drawing
                responseCurve.lineTo(x, y);
            }

            // Keep track of last in-range point
            lastX = x;
            lastY = y;
        }
    }

    // Draw the frequency response curve in yellow
    g.setColour(colors().main);
    g.strokePath(responseCurve, juce::PathStrokeType(2.0f));

    // Draw control point at cutoff frequency (reuse existing variables)
    float normCutoff = std::log(cutoff / minFreq) / std::log(maxFreq / minFreq);
    float controlX = drawArea.getX() + normCutoff * drawArea.getWidth();
    
    // Calculate magnitude at cutoff frequency for Y position
    float omega = 2.0f * juce::MathConstants<float>::pi * cutoff / sampleRate;
    std::complex<float> expNegjOmega = std::exp(std::complex<float>(0, -omega));
    std::complex<float> expNegj2Omega = std::exp(std::complex<float>(0, -2 * omega));
    std::complex<float> numerator = a0 + a1 * expNegjOmega + a2 * expNegj2Omega;
    std::complex<float> denominator = std::complex<float>(1, 0) + b1 * expNegjOmega + b2 * expNegj2Omega;
    float magnitude = std::abs(numerator / denominator);
    float dB = juce::Decibels::gainToDecibels(magnitude);
    float clampedDb = juce::jlimit(minDecibels, maxDecibels, dB);
    float controlY = juce::jmap(clampedDb, maxDecibels, minDecibels,
                                (float)drawArea.getY(), (float)drawArea.getBottom());
    
    // Draw control point circle
    g.setColour(colors().main.withAlpha(0.7f));
    g.fillEllipse(controlX - 5, controlY - 5, 10, 10);
    
    // Draw a subtle outline
    g.setColour(colors().white.withAlpha(0.5f));
    g.drawEllipse(controlX - 5, controlY - 5, 10, 10, 1.5f);

    juce::Path fillPath = responseCurve;
    fillPath.lineTo(drawArea.getRight(), drawArea.getBottom());
    fillPath.lineTo(drawArea.getX(), drawArea.getBottom());
    fillPath.closeSubPath();
    g.setGradientFill(juce::ColourGradient(
        colors().main.withAlpha(0.15f),
        drawArea.getX(), drawArea.getY(),
        colors().main.withAlpha(0.05f),
        drawArea.getX(), drawArea.getBottom(),
        false));
    g.fillPath(fillPath);

    float freq = cutoffSlider.getValue();
    float q = resonanceSlider.getValue();

    cutoffTextBox.setBounds(cutoffSlider.getX() + 2, cutoffSlider.getY() - 12, 40, 17);
    resonanceTextBox.setBounds(resonanceSlider.getX() + 2, resonanceSlider.getY() - 12, 40, 17);

}

void FilterComp::resized()
{
    // ===== copied from EnvComp.cpp =====
    auto bounds = getLocalBounds();

    auto graphHeight = bounds.getHeight() * 0.7;
    bounds.removeFromTop(graphHeight);

    auto sliderArea = bounds;
    auto sliderWidth = sliderArea.getWidth() / 2;

    auto knobSize = juce::jmin(sliderWidth * 0.6f, 45.f);

    auto cutoffArea = sliderArea.removeFromLeft(sliderWidth);
    auto resonanceArea = sliderArea;

    auto cutOffBounds = cutoffArea.withSizeKeepingCentre(knobSize, knobSize);
    cutoffSlider.setBounds(cutOffBounds);
    freqLabel.setBounds(cutOffBounds.withWidth(40).translated(-40, 0));

    auto resBounds = resonanceArea.withSizeKeepingCentre(knobSize, knobSize);
    resonanceSlider.setBounds(resBounds);
    qLabel.setBounds(resBounds.withWidth(20).translated(-20, 0));

}

void FilterComp::sliderValueChanged(juce::Slider* slider)
{
    // When either knob changes, update the display
    if (slider == &cutoffSlider)
    {
        cutoffTextBox.setText(juce::String(slider->getValue()), false);
    }
    else if (slider == &resonanceSlider)
    {
        resonanceTextBox.setText(juce::String(slider->getValue()), false);
    }
    repaint();
}

void FilterComp::onDragStart(juce::Point<int> startPos)
{
    // Store initial parameter values
    dragStartCutoff = cutoffSlider.getValue();
    dragStartResonance = resonanceSlider.getValue();
}

void FilterComp::onDragUpdate(juce::Point<int> currentPos, juce::Point<int> deltaPos)
{
    // Horizontal: frequency (logarithmic/exponential scale)
    // Moving right increases frequency, left decreases it
    const float freqSensitivity = 0.025f; // pixels to octaves (adjust for feel)
    float newCutoff = dragStartCutoff * std::pow(2.0f, deltaPos.x * freqSensitivity);
    newCutoff = juce::jlimit(20.0f, 20000.0f, newCutoff);
    cutoffSlider.setValue(newCutoff, juce::sendNotificationSync);
    
    // Vertical: Q factor (resonance)
    // Moving down (positive Y) increases Q (sharper peak)
    // Moving up (negative Y) decreases Q (broader peak)
    const float qSensitivity = 0.02f; // pixels per Q unit
    float newQ = dragStartResonance - (deltaPos.y * qSensitivity);
    newQ = juce::jlimit(0.1f, 10.0f, newQ);
    resonanceSlider.setValue(newQ, juce::sendNotificationSync);
}

void FilterComp::onDragEnd(juce::Point<int> endPos)
{
    // Optional: could add snapping, automation recording, etc.
}
