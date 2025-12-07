/*
  ==============================================================================

    OutsetVerbEngine.h
    
    Audio processing engine for Outset-Verb multi-effect processor.
    This class encapsulates all audio processing logic and can be used
    independently of JUCE's AudioProcessor framework.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include "Effects/ReverbNode.h"
#include "Effects/BitCrusherNode.h"
#include "Effects/DelayNode.h"
#include "Effects/ThreeBandEQNode.h"

//==============================================================================
/**
    Audio processing engine for Outset-Verb effects.
    
    This class manages the audio processing for all effects without being tied
    to the AudioProcessor framework, making it reusable in other contexts.
*/
class OutsetVerbEngine
{
public:
    //==============================================================================
    /** Constructor - accepts reference to external APVTS for parameter management. */
    OutsetVerbEngine(juce::AudioProcessorValueTreeState& apvtsRef);
    
    /** Destructor. */
    ~OutsetVerbEngine() = default;
    
    //==============================================================================
    /** Prepares the audio processing engine with the given specs. */
    void prepare(const juce::dsp::ProcessSpec& spec);
    
    /** Processes an audio buffer through the effect chain. */
    void processBlock(juce::AudioBuffer<float>& buffer);
    
    /** Resets all effect processors. */
    void reset();
    
    //==============================================================================
    /** Creates the parameter layout for all Outset-Verb parameters.
        This static method can be called to get the parameter layout for 
        incorporating into an APVTS. */
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:
    //==============================================================================
    // Effect type enumeration
    enum EffectType
    {
        none = 0,
        bitCrusher = 1,
        delay = 2,
        eq = 3,
        reverb = 4
    };
    
    // Individual effect processors
    BitCrusherNode bitCrusherProcessor;
    DelayNode delayProcessor;
    ThreeBandEQNode eqProcessor;
    ReverbNode reverbProcessor;
    
    // Chain configuration - stores which effect is in each position
    std::array<int, 4> chainConfiguration = {0, 0, 0, 0};
    
    // Reference to external APVTS (not owned by this class)
    juce::AudioProcessorValueTreeState& apvts;
    
    //==============================================================================
    /** Updates all effect parameters from APVTS values. */
    void updateChainParameters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutsetVerbEngine)
};
