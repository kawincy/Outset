/*
  ==============================================================================

    VoiceHandler.h
    Created: 14 Apr 2025 12:26:25am
    Author:  ryanb

  ==============================================================================
*/


#pragma once

#include "Voice.h"    // Ensure your voice.h defines the Voice class interface.
#include <vector>
#include <map>
#include <algorithm>

// VoiceHandler class manages polyphony by routing incoming note events to a collection of Voice objects.
class VoiceHandler
{
public:
    /// Constructor: Initialize the voice handler with a specified maximum polyphony.
    /// @param maxPolyphony The maximum number of simultaneous voices (default is 8).
    VoiceHandler(int maxPolyphony = 8)
        : maxPolyphony(maxPolyphony)
    {
        voices.resize(maxPolyphony);
        for (auto& voice : voices)
        {
            voice.init();
        }
    }

    /// Destructor.
    ~VoiceHandler() {}

    /// Trigger a note on event.
    /// If the note is already active, it retriggers that voice.
    /// Otherwise, it assigns the note to an available voice, or steals one if necessary.
    void noteOn(int note, int velocity)
    {
        // If the note is already active, retrigger the voice.
        if (activeNotes.find(note) != activeNotes.end())
        {
            int voiceIndex = activeNotes[note];
            voices[voiceIndex].noteOn(note, velocity);
            return;
        }
        // Find a free voice.
        Voice* voice = findFreeVoice();
        // If none is free, steal a voice.
        if (!voice)
            voice = stealVoice();

        if (voice)
        {
            // Calculate the voice index (assumes voices vector is contiguous).
            int voiceIndex = static_cast<int>(voice - &voices[0]);
            voice->noteOn(note, velocity);
            activeNotes[note] = voiceIndex;
        }
    }

    /// Trigger a note off event for the given note.
    void noteOff(int note)
    {
        auto it = activeNotes.find(note);
        if (it != activeNotes.end())
        {
            int voiceIndex = it->second;
            voices[voiceIndex].noteOff();
            activeNotes.erase(it);
        }
    }

    /// Generate the next audio sample by summing all active voice outputs.
    /// @return The mixed audio sample from all voices.
    float getNextSample()
    {
        float output = 0.f;
        for (auto& voice : voices)
        {
            output += voice.render();
        }
        return output;
    }

    /// Reset all voices and clear active note mappings.
    /// @param sampleRate The current sample rate to pass to each voice.
    void reset(float sampleRate)
    {
        for (auto& voice : voices)
        {
            voice.reset(sampleRate);
        }
        activeNotes.clear();
    }
	void resetCaches()
	{
		for (auto& voice : voices)
		{
			voice.resetCache();
		}
	}
    /// Stops all currently playing voices.
    void allNotesOff()
    {
        for (auto& voice : voices)
        {
            voice.noteOff();
        }
        activeNotes.clear();
    }
	std::vector<Voice>& getVoices() { return voices; } // Expose the voices for external access.
private:
    std::vector<Voice> voices;         // Array of voices for polyphony.
    int maxPolyphony;                  // Maximum number of voices.
    std::map<int, int> activeNotes;    // Mapping from MIDI note numbers to voice indices.

    /// Searches for a free voice (one that is not active).
    /// @return Pointer to a free Voice; otherwise nullptr if all voices are busy.
    Voice* findFreeVoice()
    {
        for (auto& voice : voices)
        {
            if (!voice.isActive())
                return &voice;
        }
        return nullptr;
    }

    /// Implements a simple voice stealing strategy by selecting the first active voice.
    /// More sophisticated voice-stealing logic can be implemented as needed.
    /// @return Pointer to a stolen Voice (after calling noteOff() on it).
    Voice* stealVoice()
    {
        for (auto& voice : voices)
        {
            if (voice.isActive())
            {
                voice.noteOff();
                return &voice;
            }
        }
        return nullptr; // This should not occur if maxPolyphony > 0.
    }

};
