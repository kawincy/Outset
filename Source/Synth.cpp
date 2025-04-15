/*
  ==============================================================================

    Synth.cpp
    Created: 9 Mar 2025 12:40:24pm
    Author:  Quincy Winkler (refactored by you)

  ==============================================================================
*/

#include "Synth.h"

Synth::Synth()
    : voiceHandler(8)   // initialize with a default polyphony of 8 voices
{
    // Previously: voice.init();
    // No additional initialization needed here—the VoiceHandler constructor builds the voices.
}

void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/)
{
    sampleRate = static_cast<float>(sampleRate_);
    // Reset all voices in the polyphonic bank with the new sample rate.
    voiceHandler.reset(sampleRate);
}

void Synth::deallocateResources()
{
    // do nothing
}

void Synth::reset()
{
    voiceHandler.reset(sampleRate);
    //noiseGen.reset();
}

void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];

    for (int sample = 0; sample < sampleCount; ++sample)
    {
        // Mix the output from all active voices.
        float output = voiceHandler.getNextSample();
        outputBufferLeft[sample] = output;

        if (outputBufferRight != nullptr)
        {
            outputBufferRight[sample] = output;
        }
        voiceHandler.resetCaches();
    }
}

void Synth::noteOn(int note, int velocity)
{
    DBG("Note on: " << note);
    // Delegate note-on to the voice handler.
    voiceHandler.noteOn(note, velocity);
}

void Synth::noteOff(int note)
{
    // Delegate note-off to the voice handler.
    voiceHandler.noteOff(note);
    DBG("NOTE OFF " << note);
}

void Synth::updateOsc(float fine, float coarse, float level, int index)
{
    // In a polyphonic setting, apply oscillator adjustments
    // to the operator with the specified index for all voices.
    //
    // Here we assume that VoiceHandler exposes its voice vector
    // via a getter like getVoices(). Adjust as needed.
    for (auto& voice : voiceHandler.getVoices())
    {
        // Adjust the operator parameters in each voice.
        // (It is assumed that each Voice contains an array of operators called op.)
        voice.op[index].updateRatio(coarse);
        voice.op[index].updateLevel(level);
    }
}

void Synth::updateADSR(float attack, float decay, float sustain, float release, int index)
{
    // Similarly, update the envelope parameters on a per-operator basis
    // across all voices.
    for (auto& voice : voiceHandler.getVoices())
    {
        voice.op[index].updateEnvParams(attack, decay, sustain, release);
    }
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0)
    {
        // Note off
    case 0x80:
        noteOff(data1 & 0x7F);
        break;

        // Note on
    case 0x90:
    {
        uint8_t note = data1 & 0x7F; // mask for safety
        uint8_t velo = data2 & 0x7F;
        if (velo > 0)
            noteOn(note, velo);
        else
            noteOff(note);
        break;
    }
    }
}
