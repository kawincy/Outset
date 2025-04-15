//
//  Colors.h
//  Outset
//
//  Created by Race Williams on 4/8/25.
//

#pragma once

#include <JuceHeader.h>

struct Colors {
    juce::Colour mainBlue = juce::Colour(0x91, 0xC9, 0xB5);
    juce::Colour accentBlue = juce::Colour(0x5B, 0x8F, 0x7E);
    juce::Colour bgColor = juce::Colour(0x1A, 0x1A, 0x1A);
};

Colors &colors();
