/*
  ==============================================================================

    CustomADSR.h
    Created: 19 Oct 2025
    Author:  ryanb

    Custom ADSR envelope that properly handles release from any phase.
    
    Key difference from juce::ADSR:
    - When noteOff() is called during decay phase, envelope uses full release 
      time to reach zero from current level (not instant jump)
    - Properly interpolates release from any starting level

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomADSR
{
public:
    struct Parameters
    {
        float attack = 0.1f;   // seconds
        float decay = 0.1f;    // seconds
        float sustain = 1.0f;  // level (0.0 to 1.0)
        float release = 0.1f;  // seconds
    };

    enum class State
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    CustomADSR() = default;

    void setSampleRate(double newSampleRate) noexcept
    {
        jassert(newSampleRate > 0.0);
        sampleRate = newSampleRate;
        updateRates();
    }

    void setParameters(const Parameters& newParams) noexcept
    {
        parameters = newParams;
        updateRates();
    }

    void noteOn() noexcept
    {
        if (parameters.attack > 0.0f)
        {
            state = State::Attack;
            level = (state == State::Attack) ? level : 0.0f; // Continue from current if retriggering
        }
        else if (parameters.decay > 0.0f)
        {
            state = State::Decay;
            level = 1.0f;
        }
        else
        {
            state = State::Sustain;
            level = parameters.sustain;
        }
    }

    void noteOff() noexcept
    {
        if (state != State::Idle)
        {
            // Store current level when entering release
            releaseStartLevel = level;
            
            if (parameters.release > 0.0f)
            {
                state = State::Release;
                updateReleaseRate(); // Calculate rate based on current level
            }
            else
            {
                state = State::Idle;
                level = 0.0f;
            }
        }
    }

    void reset() noexcept
    {
        level = 0.0f;
        state = State::Idle;
        releaseStartLevel = 0.0f;
    }

    float getNextSample() noexcept
    {
        switch (state)
        {
            case State::Idle:
                level = 0.0f;
                break;

            case State::Attack:
                level += attackRate;
                if (level >= 1.0f)
                {
                    level = 1.0f;
                    if (parameters.decay > 0.0f)
                        state = State::Decay;
                    else
                        goToSustain();
                }
                break;

            case State::Decay:
                level -= decayRate;
                if (level <= parameters.sustain)
                    goToSustain();
                break;

            case State::Sustain:
                level = parameters.sustain;
                break;

            case State::Release:
                // Release from whatever level we started at
                level -= releaseRate;
                if (level <= 0.0f)
                {
                    level = 0.0f;
                    state = State::Idle;
                }
                break;
        }

        return level;
    }

    bool isActive() const noexcept
    {
        return state != State::Idle;
    }

    State getState() const noexcept
    {
        return state;
    }

    float getCurrentLevel() const noexcept
    {
        return level;
    }

private:
    void updateRates() noexcept
    {
        if (sampleRate <= 0.0)
            return;

        // Attack: rise from 0 to 1 in attack seconds
        attackRate = (parameters.attack > 0.0f) 
            ? (1.0f / (parameters.attack * static_cast<float>(sampleRate)))
            : 0.0f;

        // Decay: fall from 1 to sustain in decay seconds
        decayRate = (parameters.decay > 0.0f)
            ? ((1.0f - parameters.sustain) / (parameters.decay * static_cast<float>(sampleRate)))
            : 0.0f;

        // Release: we'll calculate this dynamically based on starting level
        // Store the base rate (time to go from 1.0 to 0.0)
        releaseBaseRate = (parameters.release > 0.0f)
            ? (1.0f / (parameters.release * static_cast<float>(sampleRate)))
            : 0.0f;
    }

    void goToSustain() noexcept
    {
        level = parameters.sustain;
        state = State::Sustain;
    }

    // When entering release, calculate actual rate based on current level
    void updateReleaseRate() noexcept
    {
        // Scale release rate so it takes the full release time to reach 0
        // from wherever we are
        if (parameters.release > 0.0f && releaseStartLevel > 0.0f)
        {
            releaseRate = releaseStartLevel / (parameters.release * static_cast<float>(sampleRate));
        }
        else
        {
            releaseRate = releaseBaseRate;
        }
    }

    Parameters parameters;
    State state = State::Idle;
    double sampleRate = 44100.0;
    
    float level = 0.0f;
    float attackRate = 0.0f;
    float decayRate = 0.0f;
    float releaseBaseRate = 0.0f;
    float releaseRate = 0.0f;
    float releaseStartLevel = 0.0f; // Level when noteOff was called
};
