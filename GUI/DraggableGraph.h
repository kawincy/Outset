/*
  ==============================================================================

    DraggableGraph.h
    Created: 15 Oct 2025
    Author:  ryanb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
    Base class for interactive draggable graphs.
    Handles mouse interaction and provides virtual methods for subclasses
    to implement specific parameter mapping logic.
*/
class DraggableGraph : public juce::Component
{
public:
    DraggableGraph() = default;
    virtual ~DraggableGraph() override = default;

    void mouseDown(const juce::MouseEvent& event) override
    {
        if (isInteractionEnabled && graphBounds.contains(event.getPosition()))
        {
            dragStartPos = event.getPosition();
            isDragging = true;
            onDragStart(event.getPosition());
            setMouseCursor(juce::MouseCursor::DraggingHandCursor);
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (isDragging)
        {
            juce::Point<int> deltaPos = event.getPosition() - dragStartPos;
            onDragUpdate(event.getPosition(), deltaPos);
        }
    }

    void mouseUp(const juce::MouseEvent& event) override
    {
        if (isDragging)
        {
            isDragging = false;
            onDragEnd(event.getPosition());
            setMouseCursor(juce::MouseCursor::NormalCursor);
        }
    }

    void setInteractionEnabled(bool enabled) { isInteractionEnabled = enabled; }
    bool getInteractionEnabled() const { return isInteractionEnabled; }

protected:
    /**
        Called when a drag operation starts.
        Subclasses should detect which region was clicked and store initial parameter values.
        @param startPos The mouse position where the drag started
    */
    virtual void onDragStart(juce::Point<int> startPos) = 0;

    /**
        Called during drag to update parameters.
        @param currentPos Current mouse position
        @param deltaPos Delta from drag start position
    */
    virtual void onDragUpdate(juce::Point<int> currentPos, juce::Point<int> deltaPos) = 0;

    /**
        Called when drag operation ends.
        @param endPos Final mouse position
    */
    virtual void onDragEnd(juce::Point<int> endPos) = 0;

    // Subclasses should set this to define the interactive area
    juce::Rectangle<int> graphBounds;

private:
    bool isDragging = false;
    bool isInteractionEnabled = true;
    juce::Point<int> dragStartPos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableGraph)
};
