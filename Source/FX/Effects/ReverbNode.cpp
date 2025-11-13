/*
  ==============================================================================

    ReverbNode.cpp

  ==============================================================================
*/

#include "ReverbNode.h"


//==============================================================================
ReverbNode::ReverbNode()
{
    // Initialize with default reverb parameters
    currentParams.roomSize = 0.5f;
    currentParams.damping = 0.5f;
    currentParams.wetLevel = 0.33f;
    currentParams.dryLevel = 0.4f;
    currentParams.width = 1.0f;
    currentParams.freezeMode = 0.0f;
    
    // Apply initial parameters
    updateInternalReverb();
}

//==============================================================================
void ReverbNode::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;
    
    // Initialize the reverb with the sample rate
    reverb.setSampleRate(currentSampleRate);
    
    // Reset the reverb state
    reverb.reset();
    
    // Apply current parameters
    updateInternalReverb();
}

void ReverbNode::reset()
{
    reverb.reset();
}

//==============================================================================
void ReverbNode::setParameters(const juce::Reverb::Parameters& newParams)
{
    currentParams = newParams;
    updateInternalReverb();
}

void ReverbNode::setRoomSize(float roomSize)
{
    currentParams.roomSize = juce::jlimit(0.0f, 1.0f, roomSize);
    updateInternalReverb();
}

void ReverbNode::setDamping(float damping)
{
    currentParams.damping = juce::jlimit(0.0f, 1.0f, damping);
    updateInternalReverb();
}

void ReverbNode::setWetLevel(float wetLevel)
{
    currentParams.wetLevel = juce::jlimit(0.0f, 1.0f, wetLevel);
    updateInternalReverb();
}

void ReverbNode::setDryLevel(float dryLevel)
{
    currentParams.dryLevel = juce::jlimit(0.0f, 1.0f, dryLevel);
    updateInternalReverb();
}

void ReverbNode::setWidth(float width)
{
    currentParams.width = juce::jlimit(0.0f, 1.0f, width);
    updateInternalReverb();
}

void ReverbNode::setFreezeMode(float freezeMode)
{
    currentParams.freezeMode = freezeMode;
    updateInternalReverb();
}

void ReverbNode::setMix(float mix)
{
    mix = juce::jlimit(0.0f, 1.0f, mix);
    currentParams.wetLevel = mix;
    currentParams.dryLevel = 1.0f - mix;
    updateInternalReverb();
}

//==============================================================================
void ReverbNode::updateInternalReverb()
{
    reverb.setParameters(currentParams);
}
