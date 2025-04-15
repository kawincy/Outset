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
	frequency = 261.63f;
	tuning = 0;
	setFrequency(261.63f);
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
	fs = sampleRate;
	env.setSampleRate(fs);
	osc.reset();
	freqSmooth.reset(int(50));
	ampSmooth.reset(int(50));

}
void Operator::resetCache() {
	cached = false;
	//lastSample = 0.f;
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



float Operator::getNextSample()
{
	float currentEnv = env.getNextSample();
	float output = 0.f;
	float modSample = 0.f;
	if (feedback)
	{
		modSample += lastSample;
	}
	if (!modOperators.empty()) {
		for (auto mod : modOperators)
		{
			if (mod != nullptr) {
				modSample += mod->getCachedSample();
			}
		}
	}


	setFrequency(frequency + 1000 * modSample);

	ampSmooth.setTargetValue(osc.amplitude * level * currentEnv);
	output = osc.nextSample() * ampSmooth.getNextValue();
	lastSample = (output + lastSample) / 2.f; // This seeks to emulate the averaging filter of the DX7
	return output;
}

void Operator::updateRatio(float ratio_)
{
	ratio = ratio_;
	float freq = ratio * 440.0f * std::exp2(float(note - 69 + tuning) / 12.0f); // this is the midi to freq formula
	frequency = freq;
}

void Operator::updateLevel(float level_)
{
	level = level_;
}
void Operator::updateTuning(float fine, float coarse)
{
	tuning = coarse + fine / 100.0f;
	float freq = ratio * 440.0f * std::exp2(float(note - 69 + tuning) / 12.0f); // this is the midi to freq formula
	frequency = freq;
}

void Operator::noteOn(int note_, int velocity)
{
	note = note_;
	float freq = ratio * 440.0f * std::exp2(float(note - 69 + tuning) / 12.0f); // this is the midi to freq formula
	DBG("Operator " << opIndex << " initial freq: " << freq);
	//setFrequency(freq);
	frequency = freq;
	osc.amplitude = (velocity / 127.0f) * 0.5f;
	env.noteOn();
	DBG("Operator " << opIndex << " second freq: " << osc.getFrequency());

}
void Operator::noteOff()
{
	env.noteOff();
}
