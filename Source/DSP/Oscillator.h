/*
  ==============================================================================

    Oscillator.h
    Created: 9 Mar 2025 3:21:55pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#pragma once
#include <cmath>
#include <JuceHeader.h>
const float TWO_PI = 6.2831853071795864f;
class Oscillator {
public:
    float amplitude; 
//    float freq;
//    float sampleRate; 
//    float phaseOffset;
//    int sampleIndex;
    
    void reset()
    {
//        sampleIndex = 0;
        phase = 0.0f;
    }
    float nextSample()
    {
        phase += inc;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
        return std::sin(TWO_PI * phase);
//        float output = amplitude * std::sin(TWO_PI * sampleIndex * freq / sampleRate + phaseOffset);
//        sampleIndex += 1;
//        return output;
    }
    float getFrequency()
    {
        return freq;
    }
    void setFrequency(float freq_, float sampleRate)
    {
        freq = freq_;
        inc = freq / sampleRate;
    }
    float getPhase()
    {
		return phase;
    }
private:
    float phase;

    float freq;
    float inc;

};
