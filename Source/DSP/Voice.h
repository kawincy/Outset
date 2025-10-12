/*
  ==============================================================================

    Voice.h
    Created: 9 Mar 2025 12:40:33pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#pragma once

#include "Oscillator.h"
#include "Operator.h"
struct Voice {
    void init() {
        for (int i = 0; i < 6; i++)
        {
            op.emplace_back(i);
            op[i].init(i);
        }

  //      op[0].addModOperator(&op[1]);
  //      DBG("Op " << 0 << "modulated by Op " << 1);
  //      for (int i = 2; i < 5; i++) {
  //          op[i].addModOperator(&op[i + 1]);
  //          DBG("Op " << i << "modulated by Op " << i + 1);
  //      }
		//op[0].setCarrier(true);
  //      op[2].setCarrier(true);
    }
    
    void reset(float sampleRate) {
        note = -1;
        for (int i = 0; i < 6; i++)
        {
			op[i].reset(sampleRate);
            op[i].resetCache();
        }
    }

    void resetCache() {
        for (int i = 0; i < 6; i++)
        {
            op[i].resetCache();
        }
    }
    float render()
    {
		float output = 0.f;
        uint8_t numCarriers = 0;
        for (int i = 0; i < 6; i++)
        {
			if (op[i].isCarrier()) {
				numCarriers++;
				output += op[i].getNextSample();
			}
        }
		jassert(numCarriers > 0); // This shouldn't happen
		return output / float(numCarriers); // average over all carriers
    }
	void noteOn(int note_, int velocity) {
		for (int i = 0; i < 6; i++)
		{
            note = note_;

			op[i].noteOn(note_, velocity);
		}
	}
    void noteOff() {
        for (int i = 0; i < 6; i++)
            op[i].noteOff();
    }
    bool isActive() {
        int sum = 0;
        for (int i = 0; i < 6; i++) {
            if (op[i].isCarrier())
                sum += op[i].env.isActive();
        }
        if (sum > 0) {
            return true;
        }
        else
        {
            return false;
        }
    }
    int note;
//    int velocity;
    std::vector<Operator> op;
};
