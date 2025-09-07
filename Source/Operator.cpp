/*
  ==============================================================================

	Operator.cpp
	Created: 31 Mar 2025 1:37:36pm
	Author:  ryanb

  ==============================================================================
*/

#include "Operator.h"

Operator::Operator()
{
	sampleRate = 48000;
	env.setSampleRate(48000);
	env.setParameters({ 0.1f, 0.1f, 0.8f, 0.1f });
	// Initialise runtime variables to safe defaults
	note = -1;
	cached = false;
	lastSample = 0.f;
	cachedSample = 0.f;
	ratio = 1.f;
	level = 0.5f;
	tuning = 0.f;
	baseFrequency = 261.63f; // Middle C reference
}

Operator::Operator(int index)
{
	opIndex = index;
	sampleRate = 48000;
	env.setSampleRate(48000);
	env.setParameters({ 0.1f, 0.1f, 0.8f, 0.1f });
	level = 0.5f;
	osc.amplitude = 0.5f;
	ratio = 1.f;
	baseFrequency = 261.63f;
	tuning = 0;
	note = -1; // not assigned yet
	setFrequency(baseFrequency);
	cached = false;
	lastSample = 0.f;
	cachedSample = 0.f;
}

Operator::~Operator()
{
}

void Operator::init(int opIndex_) // deprecated
{
	opIndex = opIndex_;
	env.setSampleRate(48000);
	env.setParameters({ 0.1f, 0.1f, 0.8f, 0.1f });
	freqSmooth.reset(int(50));
	ampSmooth.reset(int(50));
}

void Operator::reset(float fs) {
	sampleRate = fs;
	env.setSampleRate(fs);
	osc.reset();
	freqSmooth.reset(int(50));
	ampSmooth.reset(int(50));
	// do not alter baseFrequency here; it depends on the current note/ratio/tuning
	cached = false;

}
void Operator::resetCache() {
	cached = false;
	lastSample = 0.f; // important so feedback from a previous note doesn't affect the next
	cachedSample = 0.f;
}
float Operator::getCachedSample()
{
	if (!cached)
	{
		cachedSample = getNextSample();
		cached = true;
	}
	return cachedSample;
}
void Operator::setFrequency(float freq_)
{
	//freqSmooth.setTargetValue(freq_);
	//osc.setFrequency(freqSmooth.getNextValue(), sampleRate);
	osc.setFrequency(freq_, sampleRate);
}

void Operator::updateEnvParams(float attack, float decay, float sustain, float release)
{
	//DBG("attack: " << attack);
	//DBG("decay: " << decay);
	//DBG("sustain: " << sustain);
	//DBG("Release: " << release);
	env.setParameters({ attack, decay, sustain, release });
}

void Operator::setLevel(float level_)
{
	//DBG("Level: " << level_);
	level = level_;
}

void Operator::addModOperator(Operator* mod)
{
	modOperators.push_back(mod);
}

void Operator::resetRouting()
{
	modOperators.clear();
	setFeedback(false);
	setCarrier(false);
}


float Operator::getNextSample()
{
	float currentEnv = env.getNextSample();
	float output = 0.f;
	float modSample = 0.f;
	if (feedback)
	{
		modSample += lastSample * 0.25f; // scale feedback to prevent runaway & pitch shift
	}
	if (!modOperators.empty()) {
		for (auto mod : modOperators)
		{
			if (mod != nullptr) {
				modSample += mod->getCachedSample();
			}
		}
	}
	// --- FM deviation scaling (simple): deviation proportional to base freq ---
	// Previous formula doubled baseFrequency every sample (base + base) causing octave shift.
	constexpr float modulationIndex = 1.f; // TODO: expose as parameter
	float deviation = modulationIndex * modSample * baseFrequency; // Hz deviation
	float currentFreq = baseFrequency + deviation;
	if (currentFreq < 0.f) currentFreq = 0.f;
	setFrequency(currentFreq); // do FM

	ampSmooth.setTargetValue(osc.amplitude * level * currentEnv);
	output = osc.nextSample() * ampSmooth.getNextValue();
	lastSample = (output + lastSample) / 2.f; // This seeks to emulate the averaging filter of the DX7
	return output;
}

void Operator::updateRatio(float ratio_)
{
	ratio = ratio_;
	if (note >= 0) {
		float freq = ratio * 440.0f * std::exp2(float(note - 69 + tuning) / 12.0f);
		baseFrequency = freq;
	}
}

void Operator::updateLevel(float level_)
{
	level = level_;
}
void Operator::updateTuning(float fine, float coarse)
{
	tuning = coarse + fine / 100.0f;
	if (note >= 0) {
		float freq = ratio * 440.0f * std::exp2(float(note - 69 + tuning) / 12.0f);
		baseFrequency = freq;
	}
}

void Operator::noteOn(int note_, int velocity)
{
	note = note_;
	float freq = ratio * 440.0f * std::exp2(float(note - 69 + tuning) / 12.0f); // this is the midi to freq formula
	baseFrequency = freq; // stable base
	setFrequency(baseFrequency); // ensure oscillator increment set immediately
	osc.amplitude = (velocity / 127.0f) * 0.5f;
	env.noteOn();
	lastSample = 0.f; // clear feedback for consistent retrigger
	cached = false; // force recache in modulators

}
void Operator::noteOff()
{
	env.noteOff();
}
