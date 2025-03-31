/*
  ==============================================================================

    Voice.h
    Created: 9 Mar 2025 12:40:33pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#pragma once

#include "DSP/Oscillator.h"

struct Voice {
    void reset() {
        note = -1;
//        velocity = 0;
    }
    
    float render()
    {
        return osc.nextSample();
        osc.reset();
    }
    
    int note;
//    int velocity;
    Oscillator osc;
    juce::ADSR env;
};
