/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OutsetAudioProcessor::OutsetAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    filter = std::make_unique<Filters>();
    scope = std::make_unique<Scope>();
}

OutsetAudioProcessor::~OutsetAudioProcessor()
{
}

//==============================================================================
const juce::String OutsetAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OutsetAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OutsetAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OutsetAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OutsetAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OutsetAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OutsetAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OutsetAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OutsetAudioProcessor::getProgramName (int index)
{
    return {};
}

void OutsetAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OutsetAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    filter->prepare(spec);
    synth.allocateResources(sampleRate, samplesPerBlock);
    reset();
}

void OutsetAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    synth.deallocateResources();
}

void OutsetAudioProcessor::reset()
{
    synth.reset();
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool OutsetAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OutsetAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    
    //our code (non-template stuff) starts here
  
    double cutoff = apvts.getRawParameterValue("CUTOFF")->load();
	double q = apvts.getRawParameterValue("RESONANCE")->load();
    int algIndex = apvts.getRawParameterValue("ALG_INDEX")->load();
    const int numOperators = 6;

	std::vector<float> level(numOperators);
	std::vector<float> fine(numOperators);
	std::vector<float> coarse(numOperators);
	std::vector<float> ratio(numOperators);
    std::vector<double> attack(numOperators);
	std::vector<double> decay(numOperators);
	std::vector<double> sustain(numOperators);
	std::vector<double> release(numOperators);

    for (int i = 0; i < numOperators; ++i) {
		level[i] = apvts.getRawParameterValue("LEVEL_" + juce::String(i + 1))->load();
		fine[i] = apvts.getRawParameterValue("FINE_" + juce::String(i + 1))->load();
		coarse[i] = apvts.getRawParameterValue("COARSE_" + juce::String(i + 1))->load();
		ratio[i] = apvts.getRawParameterValue("RATIO_" + juce::String(i + 1))->load();
		attack[i] = apvts.getRawParameterValue("ATTACK_" + juce::String(i + 1))->load();
		decay[i] = apvts.getRawParameterValue("DECAY_" + juce::String(i + 1))->load();
		sustain[i] = apvts.getRawParameterValue("SUSTAIN_" + juce::String(i + 1))->load();
		release[i] = apvts.getRawParameterValue("RELEASE_" + juce::String(i + 1))->load();
    }


    //DBG(cutoff);
    //DBG(attack3);
    //DBG(fine1);
    
	filter->setCutoffFrequency(cutoff);
	filter->setResonance(q);
    synth.updateAlgorithm(algIndex);
	for (int i = 0; i < 6; i++) {

		synth.updateOsc(fine[i], coarse[i], level[i], ratio[i], i);
		synth.updateADSR(attack[i], decay[i], sustain[i], release[i], i);
	}
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    splitBufferByEvents(buffer, midiMessages);
    filter->processBlock(buffer);
    
	scope->setAudioData(buffer);
    
    
    //uncomment these to check that parameters and sliders are linked


}

void OutsetAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer,
juce::MidiBuffer& midiMessages)
{
    int bufferOffset = 0;
    for (const auto metadata : midiMessages) {
        // Render the audio that happens before this event (if any).
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }
        // Handle the event. Ignore MIDI messages such as sysex.
        if (metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2);
        }
    }
    // Render the audio after the last MIDI event. If there were no
    // MIDI events at all, this renders the entire buffer.
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        render(buffer, samplesLastSegment, bufferOffset);
    }
    midiMessages.clear();
}

void OutsetAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    
    synth.midiMessage(data0, data1, data2);
    
    
    //the code below just prints all the incoming midi messages to the console. this is slow and should be commented out unless needed
//    char s[16];
//    snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
//    DBG(s);
    
    
}

void OutsetAudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset) 
{
    float* outputBuffers[2] = { nullptr, nullptr }; 
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    if (getTotalNumOutputChannels() > 1) { //conditional checks for if audio is stereo.
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    
    synth.render(outputBuffers, sampleCount);
}

//==============================================================================
bool OutsetAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OutsetAudioProcessor::createEditor()
{
    return new OutsetAudioProcessorEditor (*this, keyboardState);
}

//==============================================================================
void OutsetAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OutsetAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout OutsetAudioProcessor::createAudioParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Level Parameters (6)
    juce::NormalisableRange<float> levelRange(0.0f, 1.0f, 0.01f);
    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("LEVEL_" + juce::String(i), 1),
            "Level" + juce::String(i),
            levelRange,
            0.5f));
    }

    // Fine Parameters (6)
    juce::NormalisableRange<float> fineRange(-100.0f, 100.0f, 1.0f);
    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("FINE_" + juce::String(i), 1),
            "Fine" + juce::String(i),
            fineRange,
            0.0f));
    }

    // Coarse Parameters (6)
    juce::NormalisableRange<float> coarseRange(-12.0f, 12.0f, 1.0f);
    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("COARSE_" + juce::String(i), 1),
            "Coarse" + juce::String(i),
            coarseRange,
            0.0f));
    }
	// Ratio Parameters (6)
    auto skewRatio = 1.0f; // Set your desired midpoint value here

    juce::NormalisableRange<float> ratioRange = juce::NormalisableRange<float>(
        0.01f, 9.f,
        [skewRatio](float start, float end, float normalised)
        {
            // Apply skew first
            float skewedNormalised = normalised < 0.5f
                ? juce::jmap(normalised, 0.0f, 0.5f, 0.0f, skewRatio / (end - start))
                : juce::jmap(normalised, 0.5f, 1.0f, skewRatio / (end - start), 1.0f);

            float value = juce::jmap(skewedNormalised, start, end);

            // Apply granular increments below 2, integer increments above
            return (value < 2.0f) ? std::round(value * 100.0f) / 100.0f : std::round(value);
        },
        // Value-to-normalised lambda (with inverse skew)
        [skewRatio](float start, float end, float value)
        {
            float proportion = (value - start) / (end - start);
            float skewProportion = skewRatio / (end - start);

            float normalised = proportion < skewProportion
                ? juce::jmap(proportion, 0.0f, skewProportion, 0.0f, 0.5f)
                : juce::jmap(proportion, skewProportion, 1.0f, 0.5f, 1.0f);

            return juce::jlimit(0.0f, 1.0f, normalised);
        },
        nullptr);

    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("RATIO_" + juce::String(i), 1),
            "Ratio" + juce::String(i),
            ratioRange,
            1.0f));
    }
    // Cutoff Parameter (1)
    juce::NormalisableRange<float> cutoffRange(20.0f, 20000.0f, 1.0f);
    cutoffRange.setSkewForCentre(1000.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("CUTOFF", 1),
        "Cutoff",
        cutoffRange,
        20000.0f));

    // Resonance Parameter (1)
    juce::NormalisableRange<float> resonanceRange(0.1f, 10.0f, 0.1f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("RESONANCE", 1),
        "Resonance",
        resonanceRange,
        0.707f));

    // Attack Parameters (6)
    juce::NormalisableRange<float> attackRange(0.0f, 5.0f, 0.01f);
    attackRange.setSkewForCentre(1.0f);
    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("ATTACK_" + juce::String(i), 1),
            "Attack" + juce::String(i),
            attackRange,
            0.1f));
    }

    // Decay Parameters (6)
    juce::NormalisableRange<float> decayRange(0.0f, 5.0f, 0.01f);
    decayRange.setSkewForCentre(1.0f);
    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("DECAY_" + juce::String(i), 1),
            "Decay" + juce::String(i),
            decayRange,
            0.1f));
    }

    // Release Parameters (6)
    juce::NormalisableRange<float> releaseRange(0.0f, 5.0f, 0.01f);
    releaseRange.setSkewForCentre(1.0f);
    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("RELEASE_" + juce::String(i), 1),
            "Release" + juce::String(i),
            releaseRange,
            0.1f));
    }

    // Sustain Parameters (6)
    juce::NormalisableRange<float> sustainRange(0.0f, 1.0f, 0.01f);
    for (int i = 1; i <= 6; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("SUSTAIN_" + juce::String(i), 1),
            "Sustain" + juce::String(i),
            sustainRange,
            0.8f));
    }



    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("ALG_INDEX", 1), // Parameter ID
        "Alg Index",                       // Parameter name
        0,                                 // Minimum value
        31,                                // Maximum value
        0));                               // Default value

    return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OutsetAudioProcessor();
}
