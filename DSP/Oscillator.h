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
    
    // Render sample with optional phase modulation offset (in radians)
    float nextSample(float phaseOffsetRadians = 0.0f)
    {
        // Compute modulated phase (normalized 0-1 + offset converted from radians)
        float modulatedPhase = phase + (phaseOffsetRadians / TWO_PI);
        
        // Wrap to [0, 1) range
        modulatedPhase = modulatedPhase - std::floor(modulatedPhase);
        
        // Advance carrier phase
        phase += inc;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
        
        return std::sin(TWO_PI * modulatedPhase);
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
