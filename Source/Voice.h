/*
  ==============================================================================

    Voice.h
    Created: 9 Mar 2025 12:40:33pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#pragma once

#include "DSP/Oscillator.h"
#include "Operator.h"
struct Voice {
    void init() {
        for (int i = 0; i < 6; i++)
        {
            op.emplace_back(i);
            // example algo implementation:
        }
        op[0].setModOperator(&op[1]);
        DBG("Op " << 0 << "modulated by Op " << 1);
        for (int i = 2; i < 5; i++) {
            op[i].setModOperator(&op[i + 1]);
            DBG("Op " << i << "modulated by Op " << i + 1);
        }
    }
    
    void reset() {
        note = -1;
//        velocity = 0;
    }
    
    float render()
    {
        // example algo: ops 1/3 are carriers
        return op[0].getNextSample() + op[2].getNextSample();
        //osc.reset(); Not sure why this is here?
    }
    
    int note;
//    int velocity;
    std::vector<Operator> op;
    Oscillator osc;
    juce::ADSR env;
};
