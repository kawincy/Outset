/*
  ==============================================================================

    Synth.cpp
    Created: 9 Mar 2025 12:40:24pm
    Author:  Quincy Winkler

  ==============================================================================
*/

#include "Synth.h"


Synth::Synth()
{
//    sampleRate = 48000.0f;
}
void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/)
{
    sampleRate = static_cast<float>(sampleRate_); 
	voice.env.setSampleRate(sampleRate);
}

void Synth::deallocateResources() 
{
// do nothing
}

void Synth::reset()
{
    voice.reset();
    noiseGen.reset();
}

void Synth::render(float** outputBuffers, int sampleCount)
{ //noise rendering from book. will replace with osc code later
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];
    
    for (int sample = 0; sample < sampleCount; ++sample) {
//        float noise = noiseGen.nextValue();
        
        float output = 0.0f; 
        //if (voice.note > -1) {
		voice.osc.amplitude = voice.env.getNextSample();
        output = voice.render();
        //}
        
        outputBufferLeft[sample] = output; 
        if (outputBufferRight != nullptr) {//conditional check for stereo
            outputBufferRight[sample] = output;
        }
    }
}

void Synth::noteOn(int note, int velocity) 
{
    voice.note = note;
    
    float freq = freq = 440.0f * std::exp2(float(note - 69) / 12.0f); //this is the midi to freq formula
    
	voice.env.noteOn();

    voice.osc.amplitude = (velocity / 127.0f) * 0.5f;
    voice.osc.inc = freq/sampleRate;
//    voice.osc.freq = 261.63f; //middle c
//    voice.osc.sampleRate = sampleRate;
//    voice.osc.phaseOffset = 0.0f;
    voice.osc.reset();
}
void Synth::noteOff(int note) 
{
    if (voice.note == note) { 
        voice.note = -1;
		voice.env.noteOff();
        //voice.osc.amplitude = 0; // not from the book. put this here to make the sin wave turn off
//        voice.velocity = 0;
    }
}

void Synth::updateADSR(float attack, float decay, float sustain, float release)
{
	voice.env.setParameters({ attack, decay, sustain, release });
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0) { 
        // Note off
        case 0x80:
            noteOff(data1 & 0x7F);
            break;

        // Note on
        case 0x90:
            uint8_t note = data1 & 0x7F; //this mask in an abundance of caution
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note, velo);
            } else {
                noteOff(note);
            }
            break;
    }
}

