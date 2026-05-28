/*
  ==============================================================================

    FilterComp.h
    Created: 2 Feb 2025 12:46:44pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DraggableGraph.h"
#include "rta.h"

class FilterComp : public DraggableGraph,
  private juce::Slider::Listener,
  private juce::Timer
{
public:
    FilterComp(juce::AudioProcessorValueTreeState& apvtsRef, RTA& rtaRef);
    ~FilterComp() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Slider callback
    void sliderValueChanged(juce::Slider* slider) override;

    // Optionally, update the sample rate used for response calculation
    void setSampleRate(float newSampleRate) { sampleRate = newSampleRate; repaint(); }

protected:
    // DraggableGraph overrides
    void onDragStart(juce::Point<int> startPos) override;
    void onDragUpdate(juce::Point<int> currentPos, juce::Point<int> deltaPos) override;
    void onDragEnd(juce::Point<int> endPos) override;
  void timerCallback() override { repaint(); }

private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    RTA& rta;
    
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    
    juce::AudioProcessorValueTreeState::SliderAttachment cutoffAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment resonanceAttachment;

    juce::Label freqLabel;
    juce::Label qLabel;

    juce::TextEditor cutoffTextBox;
    juce::TextEditor resonanceTextBox;

    // Sample rate used for computing the response curve (default: 48000 Hz)
    // Need to implement some sort of update for this ideally
    float sampleRate = 48000;

    // Drag state for interactive graph
    float dragStartCutoff = 1000.0f;
    float dragStartResonance = 0.707f;
    std::vector<float> spectrumCache; // for drawing

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterComp)
};

