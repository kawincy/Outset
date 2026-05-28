/*
  ==============================================================================
    ThemePicker.h
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Colors.h"

//==============================================================================
class ThemePicker : public juce::Component
{
public:
    std::function<void()> onThemeChanged;

    ThemePicker()
    {
        themes = Colors::builtinThemes();
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(colors().bg.brighter(0.08f));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);

        auto bounds    = getLocalBounds().reduced(4, 3);
        int swatchSize = juce::jmin(bounds.getHeight(), bounds.getWidth() / (int)themes.size()) - 2;
        int totalW     = (int)themes.size() * (swatchSize + swatchGap) - swatchGap;
        int startX     = bounds.getX() + (bounds.getWidth() - totalW) / 2;
        int y          = bounds.getCentreY() - swatchSize / 2;

        for (int i = 0; i < (int)themes.size(); ++i)
        {
            juce::Rectangle<int> swatch(startX + i * (swatchSize + swatchGap), y,
                                        swatchSize, swatchSize);
            swatchBounds[i] = swatch;

            g.setColour(themes[i].main);
            g.fillEllipse(swatch.toFloat());

            g.setColour(themes[i].accent);
            g.drawEllipse(swatch.toFloat().reduced(1.0f), 1.5f);

            if (i == colors().currentThemeIndex)
            {
                g.setColour(colors().white);
                g.drawEllipse(swatch.toFloat().expanded(2.0f), 2.0f);
            }
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        for (int i = 0; i < (int)themes.size(); ++i)
        {
            if (swatchBounds[i].contains(e.getPosition()))
            {
                colors().applyTheme(themes[i]);
                colors().currentThemeIndex = i;
                repaint();
                if (onThemeChanged) onThemeChanged();
                return;
            }
        }
    }

private:
    juce::Array<ColorTheme>  themes;
    juce::Rectangle<int>     swatchBounds[8];
    static constexpr int     swatchGap = 5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemePicker)
};