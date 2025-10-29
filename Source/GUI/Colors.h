 /*
  ==============================================================================

    Colors.h
    Created: 8 Apr 2025 1:27:11pm
    Author:  Race Williams

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// A singleton for Juce colours.
// If we want to implement color schemes later, we just need to update this object,
// not every single UI object independently.
struct Colors {
    juce::Colour main = juce::Colour(0x91, 0xC9, 0xB5);
    juce::Colour accent = juce::Colour(0x5B, 0x8F, 0x7E);
    juce::Colour bg = juce::Colour(0x1A, 0x1A, 0x1A);
    
    juce::Colour white = juce::Colour(0xFF, 0xFF, 0xFF);
    juce::Colour black = juce::Colour(0x00, 0x00, 0x00);
    
    juce::Colour gridlines = white.withAlpha(0.2f);
};

Colors &colors();
