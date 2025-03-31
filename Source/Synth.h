/*
  ==============================================================================

    Synth.h
    Created: 9 Mar 2025 12:40:24pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "Voice.h"
#include "DSP/NoiseGenerator.h"

class Synth {
public:
    Synth();
    void allocateResources(double sampleRate, int samplesPerBlock);
    void deallocateResources();
    void reset();
    void render(float** outputBuffers, int sampleCount);
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
    void updateADSR(float attack, float decay, float sustain, float release); // May need an additional int input for what oscillator is being updated depending on our desired topology

private:
    void noteOn(int note, int velocity);
    void noteOff(int note);
    
    float sampleRate;
    Voice voice; //will eventually be a collection of voices. likely a vector
    NoiseGenerator noiseGen;
};
