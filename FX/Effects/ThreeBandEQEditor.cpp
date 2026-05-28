/*
  ==============================================================================
    ThreeBandEQEditor.cpp
  ==============================================================================
*/

#include "ThreeBandEQEditor.h"
#include <cmath>

//==============================================================================
ThreeBandEQEditor::ThreeBandEQEditor(ThreeBandEQNode& nodeToControl,
                                      juce::AudioProcessorValueTreeState& apvts)
    : node(nodeToControl), apvts(apvts)
{
    setSize(600, 300);

    // Expand button — bottom-right corner
    expandButton.setButtonText("+");
    expandButton.setColour(juce::TextButton::buttonColourId,   juce::Colour(0xff2a2a3e));
    expandButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff4a4a6e));
    expandButton.setColour(juce::TextButton::textColourOffId,  juce::Colour(0xff00c8ff));
    expandButton.setColour(juce::TextButton::textColourOnId,   juce::Colour(0xff00c8ff));
    expandButton.setTooltip("Expand / collapse EQ editor");

    expandButton.onClick = [this]()
    {
        expanded = !expanded;
        expandButton.setButtonText(expanded ? "-" : "+");
        if (onExpandToggled)
            onExpandToggled(expanded);
    };

    addAndMakeVisible(expandButton);
}

//==============================================================================
// Layout helper — EQ drawing area excludes the button strip at the bottom
//==============================================================================

juce::Rectangle<int> ThreeBandEQEditor::getEQBounds() const noexcept
{
    // Reserve a thin strip at the bottom for the expand button
    return getLocalBounds().withTrimmedBottom(buttonSize + buttonMargin * 2);
}

//==============================================================================
// paint / resized
//==============================================================================

void ThreeBandEQEditor::paint(juce::Graphics& g)
{
    // Background for the whole component
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Subtle separator line above the button strip
    const auto eqB = getEQBounds();
    g.setColour(juce::Colour(0xff2e2e4a));
    g.drawHorizontalLine(eqB.getBottom(), 0.0f, static_cast<float>(getWidth()));

    drawGrid(g);
    drawResponseCurve(g);
    drawBandHandles(g);
}

void ThreeBandEQEditor::resized()
{
    // Place the expand button in the bottom-right corner
    expandButton.setBounds(getWidth()  - buttonSize - buttonMargin,
                           getHeight() - buttonSize - buttonMargin,
                           buttonSize, buttonSize);
    repaint();
}

//==============================================================================
// APVTS write-back
//==============================================================================

void ThreeBandEQEditor::writeToApvts(int bandIndex, float freqHz, float gainDb)
{
    auto setParam = [&](const juce::String& id, float value)
    {
        if (auto* p = apvts.getParameter(id))
        {
            const auto norm = p->getNormalisableRange().convertTo0to1(value);
            p->beginChangeGesture();
            p->setValueNotifyingHost(norm);
            p->endChangeGesture();
        }
    };

    switch (bandIndex)
    {
        case 0:
            setParam("lowFreq", freqHz);
            setParam("lowGain", gainDb);
            break;
        case 1:
            setParam("midFreq", freqHz);
            setParam("midGain", gainDb);
            break;
        case 2:
            setParam("highFreq", freqHz);
            setParam("highGain", gainDb);
            break;
        default:
            break;
    }
}

//==============================================================================
// Mouse interaction
//==============================================================================

void ThreeBandEQEditor::mouseDown(const juce::MouseEvent& e)
{
    // Let the button handle its own clicks
    if (expandButton.getBounds().contains(e.getPosition()))
        return;

    const auto pos = e.position;
    int hitBand = -1;

    if (hitTestBand(pos, hitBand))
    {
        if (e.mods.isRightButtonDown())
        {
            const float defaultFreqs[] = { 200.0f, 1000.0f, 8000.0f };
            node.setBandPoint(hitBand, defaultFreqs[hitBand], 0.0f);
            writeToApvts(hitBand, defaultFreqs[hitBand], 0.0f);

            if (hitBand == 1)
            {
                node.setMidQ(1.0f);
                if (auto* p = apvts.getParameter("midQ"))
                {
                    const auto norm = p->getNormalisableRange().convertTo0to1(1.0f);
                    p->beginChangeGesture();
                    p->setValueNotifyingHost(norm);
                    p->endChangeGesture();
                }
            }
            repaint();
            return;
        }

        draggedBand   = hitBand;
        dragStartPos  = pos;
        dragStartBand = node.getBandPoint(hitBand);
    }
}

void ThreeBandEQEditor::mouseDrag(const juce::MouseEvent& e)
{
    if (draggedBand < 0)
        return;

    const float newFreq = xToFreq(e.position.x);
    const float newGain = yToGain(e.position.y);

    node.setBandPoint(draggedBand, newFreq, newGain);
    writeToApvts(draggedBand, newFreq, newGain);
    repaint();
}

void ThreeBandEQEditor::mouseUp(const juce::MouseEvent&)
{
    draggedBand = -1;
}

void ThreeBandEQEditor::mouseWheelMove(const juce::MouseEvent& e,
                                        const juce::MouseWheelDetails& wheel)
{
    int hitBand = -1;
    if (hitTestBand(e.position, hitBand) && hitBand == 1)
    {
        node.adjustMidQ(wheel.deltaY * 0.5f);

        if (auto* p = apvts.getParameter("midQ"))
        {
            const float newQ = node.getBandPoint(1).q;
            const auto norm  = p->getNormalisableRange().convertTo0to1(newQ);
            p->beginChangeGesture();
            p->setValueNotifyingHost(norm);
            p->endChangeGesture();
        }
        repaint();
    }
}

//==============================================================================
// Coordinate mapping — all relative to getEQBounds()
//==============================================================================

float ThreeBandEQEditor::freqToX(float freqHz) const noexcept
{
    const float w      = static_cast<float>(getEQBounds().getWidth());
    const float left   = static_cast<float>(getEQBounds().getX());
    const float logMin = std::log10(minFreq);
    const float logMax = std::log10(maxFreq);
    return left + (std::log10(juce::jlimit(minFreq, maxFreq, freqHz)) - logMin)
                  / (logMax - logMin) * w;
}

float ThreeBandEQEditor::gainToY(float gainDb) const noexcept
{
    const auto  eqB = getEQBounds();
    const float h   = static_cast<float>(eqB.getHeight());
    const float top = static_cast<float>(eqB.getY());
    return top + h * 0.5f - (gainDb / maxGainDb) * (h * 0.5f);
}

float ThreeBandEQEditor::xToFreq(float x) const noexcept
{
    const auto  eqB   = getEQBounds();
    const float w     = static_cast<float>(eqB.getWidth());
    const float left  = static_cast<float>(eqB.getX());
    const float logMin = std::log10(minFreq);
    const float logMax = std::log10(maxFreq);
    const float t     = juce::jlimit(0.0f, 1.0f, (x - left) / w);
    return std::pow(10.0f, logMin + t * (logMax - logMin));
}

float ThreeBandEQEditor::yToGain(float y) const noexcept
{
    const auto  eqB = getEQBounds();
    const float h   = static_cast<float>(eqB.getHeight());
    const float top = static_cast<float>(eqB.getY());
    return juce::jlimit(-maxGainDb, maxGainDb,
                        (top + h * 0.5f - y) / (h * 0.5f) * maxGainDb);
}

//==============================================================================
// Hit-testing
//==============================================================================

bool ThreeBandEQEditor::hitTestBand(juce::Point<float> pos, int& bandIndex) const noexcept
{
    // Don't hit-test inside the button area
    if (expandButton.getBounds().toFloat().contains(pos))
        return false;

    for (int b = 0; b < 3; ++b)
    {
        const auto  bp = node.getBandPoint(b);
        const float hx = freqToX(bp.freqHz);
        const float hy = gainToY(bp.gainDb);
        if (pos.getDistanceFrom({ hx, hy }) <= handleRadius + 2.0f)
        {
            bandIndex = b;
            return true;
        }
    }
    return false;
}

//==============================================================================
// Drawing — all relative to getEQBounds()
//==============================================================================

void ThreeBandEQEditor::drawGrid(juce::Graphics& g) const
{
    const auto eqB = getEQBounds();
    const int  w   = eqB.getWidth();
    const int  h   = eqB.getHeight();
    const int  ox  = eqB.getX();
    const int  oy  = eqB.getY();

    const juce::Colour gridColour  = juce::Colour(0xff2e2e4a);
    const juce::Colour zeroColour  = juce::Colour(0xff4a4a6a);
    const juce::Colour labelColour = juce::Colour(0xff7070a0);
    g.setFont(10.0f);

    for (float db = -12.0f; db <= 12.0f; db += 3.0f)
    {
        const float y = gainToY(db);
        g.setColour(db == 0.0f ? zeroColour : gridColour);
        g.drawHorizontalLine(static_cast<int>(y),
                             static_cast<float>(ox),
                             static_cast<float>(ox + w));

        if (db != 0.0f)
        {
            g.setColour(labelColour);
            g.drawText(juce::String(static_cast<int>(db)) + " dB",
                       ox + 2, static_cast<int>(y) - 7, 36, 14,
                       juce::Justification::left, false);
        }
    }

    const float freqMarkers[] = { 50, 100, 200, 500, 1000, 2000, 5000, 10000 };
    for (auto freq : freqMarkers)
    {
        const float x = freqToX(freq);
        g.setColour(gridColour);
        g.drawVerticalLine(static_cast<int>(x),
                           static_cast<float>(oy),
                           static_cast<float>(oy + h));

        g.setColour(labelColour);
        const juce::String label = freq >= 1000.0f
            ? juce::String(static_cast<int>(freq / 1000)) + "k"
            : juce::String(static_cast<int>(freq));
        g.drawText(label,
                   static_cast<int>(x) + 2, oy + h - 16, 32, 14,
                   juce::Justification::left, false);
    }
}

float ThreeBandEQEditor::evaluateMagnitude(float freqHz) const noexcept
{
    constexpr double SR = 44100.0;
    float magnitudeDb = 0.0f;

    {
        const auto bp    = node.getBandPoint(0);
        auto       coeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            SR, bp.freqHz, 0.707f, juce::Decibels::decibelsToGain(bp.gainDb));
        magnitudeDb += juce::Decibels::gainToDecibels(
            static_cast<float>(coeffs->getMagnitudeForFrequency(
                static_cast<double>(freqHz), SR)));
    }
    {
        const auto bp    = node.getBandPoint(1);
        auto       coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            SR, bp.freqHz, bp.q, juce::Decibels::decibelsToGain(bp.gainDb));
        magnitudeDb += juce::Decibels::gainToDecibels(
            static_cast<float>(coeffs->getMagnitudeForFrequency(
                static_cast<double>(freqHz), SR)));
    }
    {
        const auto bp    = node.getBandPoint(2);
        auto       coeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            SR, bp.freqHz, 0.707f, juce::Decibels::decibelsToGain(bp.gainDb));
        magnitudeDb += juce::Decibels::gainToDecibels(
            static_cast<float>(coeffs->getMagnitudeForFrequency(
                static_cast<double>(freqHz), SR)));
    }

    return magnitudeDb;
}

void ThreeBandEQEditor::drawResponseCurve(juce::Graphics& g) const
{
    const auto  eqB  = getEQBounds();
    const float w    = static_cast<float>(eqB.getWidth());
    const float left = static_cast<float>(eqB.getX());
    juce::Path  curve;

    for (int i = 0; i < curveResolution; ++i)
    {
        const float t    = static_cast<float>(i) / static_cast<float>(curveResolution - 1);
        const float freq = std::pow(10.0f,
            std::log10(minFreq) + t * (std::log10(maxFreq) - std::log10(minFreq)));
        const float x = left + t * w;
        const float y = gainToY(evaluateMagnitude(freq));

        if (i == 0) curve.startNewSubPath(x, y);
        else        curve.lineTo(x, y);
    }

    juce::Path filled = curve;
    const float midY  = gainToY(0.0f);
    filled.lineTo(left + w, midY);
    filled.lineTo(left,     midY);
    filled.closeSubPath();

    g.setColour(juce::Colour(0x2200c8ff));
    g.fillPath(filled);
    g.setColour(juce::Colour(0xff00c8ff));
    g.strokePath(curve, juce::PathStrokeType(1.8f));
}

void ThreeBandEQEditor::drawBandHandles(juce::Graphics& g) const
{
    const juce::Colour bandColours[] =
    {
        juce::Colour(0xffff8c00),
        juce::Colour(0xff00c8ff),
        juce::Colour(0xff00e676)
    };
    const char* bandLabels[] = { "L", "M", "H" };

    for (int b = 0; b < 3; ++b)
    {
        const auto  bp        = node.getBandPoint(b);
        const float hx        = freqToX(bp.freqHz);
        const float hy        = gainToY(bp.gainDb);
        const bool  isDragged = (b == draggedBand);

        if (isDragged)
        {
            g.setColour(bandColours[b].withAlpha(0.3f));
            g.fillEllipse(hx - handleRadius * 2.0f, hy - handleRadius * 2.0f,
                          handleRadius * 4.0f,       handleRadius * 4.0f);
        }

        g.setColour(isDragged ? bandColours[b] : bandColours[b].withAlpha(0.75f));
        g.fillEllipse(hx - handleRadius, hy - handleRadius,
                      handleRadius * 2.0f, handleRadius * 2.0f);

        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.drawEllipse(hx - handleRadius, hy - handleRadius,
                      handleRadius * 2.0f, handleRadius * 2.0f, 1.2f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        g.drawText(bandLabels[b],
                   static_cast<int>(hx - handleRadius),
                   static_cast<int>(hy - handleRadius),
                   static_cast<int>(handleRadius * 2.0f),
                   static_cast<int>(handleRadius * 2.0f),
                   juce::Justification::centred, false);

        if (isDragged)
        {
            const juce::String freqStr = bp.freqHz < 1000.0f
                ? juce::String(static_cast<int>(bp.freqHz)) + " Hz"
                : juce::String(bp.freqHz / 1000.0f, 2) + " kHz";
            const juce::String gainStr = (bp.gainDb >= 0.0f ? "+" : "")
                + juce::String(bp.gainDb, 1) + " dB";
            const juce::String tip = freqStr + "  " + gainStr;

            const float tx = juce::jlimit(0.0f,
                                          static_cast<float>(getWidth()) - 100.0f,
                                          hx + 10.0f);
            const float ty = juce::jlimit(0.0f,
                                          static_cast<float>(getHeight()) - 20.0f,
                                          hy - 20.0f);
            g.setColour(juce::Colour(0xcc000000));
            g.fillRoundedRectangle(tx - 2.0f, ty - 2.0f, 100.0f, 18.0f, 3.0f);
            g.setColour(juce::Colours::white);
            g.setFont(10.0f);
            g.drawText(tip, static_cast<int>(tx), static_cast<int>(ty), 100, 14,
                       juce::Justification::left, false);
        }
    }
}