/*
  ==============================================================================
    Colors.h
    Created: 8 Apr 2025 1:27:11pm
    Author:  Race Williams
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ColorTheme
{
    juce::String name;
    juce::Colour main;
    juce::Colour accent;
    juce::Colour bg;
    juce::Colour white;
    juce::Colour black;
};

// A singleton for JUCE colours.
// Switching themes just calls colors().applyTheme(...) and repaints the top-level component.
struct Colors
{
    juce::Colour main    = juce::Colour(0x91, 0xC9, 0xB5);
    juce::Colour accent  = juce::Colour(0x5B, 0x8F, 0x7E);
    juce::Colour bg      = juce::Colour(0x1A, 0x1A, 0x1A);
    juce::Colour white   = juce::Colour(0xFF, 0xFF, 0xFF);
    juce::Colour black   = juce::Colour(0x00, 0x00, 0x00);
    juce::Colour gridlines = white.withAlpha(0.2f);

    // Built-in themes
    static juce::Array<ColorTheme> builtinThemes()
    {
        juce::Array<ColorTheme> themes;

        themes.add({ "Teal (Default)",
            juce::Colour(0x91, 0xC9, 0xB5),
            juce::Colour(0x5B, 0x8F, 0x7E),
            juce::Colour(0x1A, 0x1A, 0x1A),
            juce::Colour(0xFF, 0xFF, 0xFF),
            juce::Colour(0x00, 0x00, 0x00) });

    themes.add({ "Midnight Blue",
        juce::Colour(0x7E, 0xA8, 0xD8),
        juce::Colour(0x3A, 0x5F, 0x8F),
        juce::Colour(0x0D, 0x0D, 0x1A),
        juce::Colour(0xFF, 0xFF, 0xFF),
        juce::Colour(0x00, 0x00, 0x00) });

    themes.add({ "Ember",
        juce::Colour(0xFF, 0x8C, 0x42),
        juce::Colour(0xCC, 0x4E, 0x00),
        juce::Colour(0x1A, 0x10, 0x0A),
        juce::Colour(0xFF, 0xFF, 0xFF),
        juce::Colour(0x00, 0x00, 0x00) });

    themes.add({ "Violet",
        juce::Colour(0xBB, 0x86, 0xFC),
        juce::Colour(0x7C, 0x4D, 0xFF),
        juce::Colour(0x12, 0x0D, 0x1A),
        juce::Colour(0xFF, 0xFF, 0xFF),
        juce::Colour(0x00, 0x00, 0x00) });

    themes.add({ "Slate",
        juce::Colour(0xA0, 0xAE, 0xC0),
        juce::Colour(0x60, 0x72, 0x8A),
        juce::Colour(0x16, 0x18, 0x1C),
        juce::Colour(0xFF, 0xFF, 0xFF),
        juce::Colour(0x00, 0x00, 0x00) });

    themes.add({ "Rose",
        juce::Colour(0xFF, 0x85, 0xA1),
        juce::Colour(0xC2, 0x3B, 0x5E),
        juce::Colour(0x1A, 0x0D, 0x11),
        juce::Colour(0xFF, 0xFF, 0xFF),
        juce::Colour(0x00, 0x00, 0x00) });

    return themes;
}

    void applyTheme(const ColorTheme& t)
    {
        main      = t.main;
        accent    = t.accent;
        bg        = t.bg;
        white     = t.white;
        black     = t.black;
        gridlines = white.withAlpha(0.2f);
    }

    int currentThemeIndex = 0;
};

Colors& colors();