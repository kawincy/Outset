/*
  ==============================================================================
    ThreeBandEQEditor.h
  ==============================================================================
*/
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "ThreeBandEQNode.h"

//==============================================================================
class ThreeBandEQEditor : public juce::Component
{
public:
    explicit ThreeBandEQEditor(ThreeBandEQNode& nodeToControl,
                                juce::AudioProcessorValueTreeState& apvts);
    ~ThreeBandEQEditor() override = default;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e,
                        const juce::MouseWheelDetails& wheel) override;

    //==============================================================================
    // Expand / collapse toggle
    // FXComp sets this callback and responds by calling resized() with new bounds
    std::function<void(bool isExpanded)> onExpandToggled;

    bool isExpanded() const noexcept { return expanded; }

private:
    //==============================================================================
    ThreeBandEQNode& node;
    juce::AudioProcessorValueTreeState& apvts;

    // Expand state
    bool expanded { false };

    // Expand button — drawn manually in the bottom-right corner of the editor
    // We use a juce::DrawableButton so it sits on top of the canvas and captures clicks
    juce::TextButton expandButton;

    static constexpr int   buttonSize    = 18; // px, square
    static constexpr int   buttonMargin  = 4;

    //==========================================================================
    // Drag state
    int draggedBand = -1;
    juce::Point<float>        dragStartPos;
    ThreeBandEQNode::BandPoint dragStartBand;

    //==============================================================================
    void writeToApvts(int bandIndex, float freqHz, float gainDb);

    //==============================================================================
    // Returns the sub-rectangle used for EQ drawing
    // (excludes the button area so the button never occludes handles)
    juce::Rectangle<int> getEQBounds() const noexcept;

    float freqToX(float freqHz) const noexcept;
    float gainToY(float gainDb) const noexcept;
    float xToFreq(float x)     const noexcept;
    float yToGain(float y)     const noexcept;

    void drawGrid(juce::Graphics& g)          const;
    void drawResponseCurve(juce::Graphics& g)  const;
    void drawBandHandles(juce::Graphics& g)    const;

    float evaluateMagnitude(float freqHz) const noexcept;

    bool hitTestBand(juce::Point<float> pos, int& bandIndex) const noexcept;

    //==============================================================================
    static constexpr float minFreq       = 20.0f;
    static constexpr float maxFreq       = 20000.0f;
    static constexpr float maxGainDb     = 12.0f;
    static constexpr float handleRadius  = 7.0f;
    static constexpr int   curveResolution = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeBandEQEditor)
};