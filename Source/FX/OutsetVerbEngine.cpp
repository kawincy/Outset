/*
  ==============================================================================

    OutsetVerbEngine.cpp
    
    Audio processing engine implementation for Outset-Verb.

  ==============================================================================
*/

#include "OutsetVerbEngine.h"

//==============================================================================
OutsetVerbEngine::OutsetVerbEngine(juce::AudioProcessorValueTreeState& apvtsRef)
    : apvts(apvtsRef)
{
}

//==============================================================================
void OutsetVerbEngine::prepare(const juce::dsp::ProcessSpec& spec)
{
    // Prepare individual effect processors with the given audio specs
    bitCrusherProcessor.prepare(spec);
    delayProcessor.prepare(spec);
    eqProcessor.prepare(spec);
    reverbProcessor.prepare(spec);

    // Update parameters to current APVTS values
    updateChainParameters();
}

void OutsetVerbEngine::processBlock(juce::AudioBuffer<float>& buffer)
{
    auto numSamples = buffer.getNumSamples();
    
    // Early exit if no samples
    if (numSamples == 0)
        return;

    // Update parameters from APVTS
    updateChainParameters();

    // Create audio block from buffer for DSP processing
    juce::dsp::AudioBlock<float> audioBlock(buffer);

    // Process through effects in the configured order
    for (int slot = 0; slot < 4; ++slot)
    {
        int effectType = chainConfiguration[slot];

        // Skip if no effect selected for this slot
        if (effectType == EffectType::none)
            continue;

        // Create process context for this effect
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);

        // Process through the appropriate effect
        switch (effectType)
        {
            case EffectType::bitCrusher:
                bitCrusherProcessor.process(context);
                break;
            case EffectType::delay:
                delayProcessor.process(context);
                break;
            case EffectType::eq:
                eqProcessor.process(context);
                break;
            case EffectType::reverb:
                reverbProcessor.process(context);
                break;
            default:
                // Unknown effect type - skip
                break;
        }
    }
}

void OutsetVerbEngine::reset()
{
    // Reset individual effect processors
    bitCrusherProcessor.reset();
    delayProcessor.reset();
    eqProcessor.reset();
    reverbProcessor.reset();
}

//==============================================================================
void OutsetVerbEngine::updateChainParameters()
{
    // Update BitCrusher parameters
    bitCrusherProcessor.setBitDepth(apvts.getRawParameterValue("bitDepth")->load());
    bitCrusherProcessor.setSampleRateReduction(apvts.getRawParameterValue("sampleRateReduction")->load());
    bitCrusherProcessor.setMix(apvts.getRawParameterValue("bitCrusherMix")->load());

    // Update Delay parameters
    delayProcessor.setDelayTime(apvts.getRawParameterValue("delayTime")->load());
    delayProcessor.setFeedback(apvts.getRawParameterValue("delayFeedback")->load());
    delayProcessor.setMix(apvts.getRawParameterValue("delayMix")->load());
    delayProcessor.setLowPassCutoff(apvts.getRawParameterValue("delayLowPassCutoff")->load());

    // Update EQ parameters
    eqProcessor.setLowGain(apvts.getRawParameterValue("lowGain")->load());
    eqProcessor.setLowFreq(apvts.getRawParameterValue("lowFreq")->load());
    eqProcessor.setMidGain(apvts.getRawParameterValue("midGain")->load());
    eqProcessor.setMidFreq(apvts.getRawParameterValue("midFreq")->load());
    eqProcessor.setMidQ(apvts.getRawParameterValue("midQ")->load());
    eqProcessor.setHighGain(apvts.getRawParameterValue("highGain")->load());
    eqProcessor.setHighFreq(apvts.getRawParameterValue("highFreq")->load());

    // Update Reverb parameters
    reverbProcessor.setRoomSize(apvts.getRawParameterValue("roomSize")->load());
    reverbProcessor.setDamping(apvts.getRawParameterValue("damping")->load());
    reverbProcessor.setWidth(apvts.getRawParameterValue("width")->load());

    // Handle freeze mode - convert bool to float
    bool freezeMode = apvts.getRawParameterValue("freezeMode")->load() > 0.5f;
    reverbProcessor.setFreezeMode(freezeMode ? 1.0f : 0.0f);

    // Handle reverb mix parameter
    float reverbMixValue = apvts.getRawParameterValue("reverbMix")->load();
    reverbProcessor.setMix(reverbMixValue);

    // Update chain configuration from parameters
    chainConfiguration[0] = static_cast<int>(apvts.getRawParameterValue("chainSlot1")->load());
    chainConfiguration[1] = static_cast<int>(apvts.getRawParameterValue("chainSlot2")->load());
    chainConfiguration[2] = static_cast<int>(apvts.getRawParameterValue("chainSlot3")->load());
    chainConfiguration[3] = static_cast<int>(apvts.getRawParameterValue("chainSlot4")->load());
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout OutsetVerbEngine::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // BitCrusher parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bitDepth", 1),
        "Bit Depth",
        juce::NormalisableRange<float>(1.0f, 16.0f, 1.0f),
        16.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("sampleRateReduction", 1),
        "Sample Rate Reduction",
        juce::NormalisableRange<float>(1.0f, 50.0f, 1.0f),
        1.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bitCrusherMix", 1),
        "BitCrusher Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f)
    );

    // Delay parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayTime", 1),
        "Delay Time",
        juce::NormalisableRange<float>(0.0f, 2000.0f, 1.0f),
        250.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayFeedback", 1),
        "Delay Feedback",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f),
        0.3f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayMix", 1),
        "Delay Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayLowPassCutoff", 1),
        "Delay Low Pass",
        juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f),
        8000.0f)
    );

    // EQ parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lowGain", 1),
        "Low Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lowFreq", 1),
        "Low Freq",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f),
        200.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midGain", 1),
        "Mid Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midFreq", 1),
        "Mid Freq",
        juce::NormalisableRange<float>(200.0f, 5000.0f, 1.0f),
        1000.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midQ", 1),
        "Mid Q",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f),
        1.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("highGain", 1),
        "High Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("highFreq", 1),
        "High Freq",
        juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f),
        8000.0f)
    );

    // Reverb parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("roomSize", 1),
        "Room Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("damping", 1),
        "Dampening",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbMix", 1),
        "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("width", 1),
        "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("freezeMode", 1),
        "Freeze",
        false)
    );

    // Chain configuration parameters
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("chainSlot1", 1),
        "Chain Slot 1",
        juce::StringArray{"None", "Bit Crusher", "Delay", "EQ", "Reverb"},
        0)  // Default: None
    );

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("chainSlot2", 1),
        "Chain Slot 2",
        juce::StringArray{"None", "Bit Crusher", "Delay", "EQ", "Reverb"},
        0)  // Default: None
    );

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("chainSlot3", 1),
        "Chain Slot 3",
        juce::StringArray{"None", "Bit Crusher", "Delay", "EQ", "Reverb"},
        0)  // Default: None
    );

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("chainSlot4", 1),
        "Chain Slot 4",
        juce::StringArray{"None", "Bit Crusher", "Delay", "EQ", "Reverb"},
        0)  // Default: None
    );

    return layout;
}
