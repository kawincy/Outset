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
	setFrequency(261);
}

Operator::~Operator()
{
}

void Operator::init(int opIndex_)
{
	opIndex = opIndex_;
	env.setSampleRate(48000);
	env.setParameters({ 0.1f, 0.1f, 0.8f, 0.1f });
}

void Operator::reset(float fs) {
	fs = sampleRate;
	env.setSampleRate(fs);
	osc.reset();
}

void Operator::setFrequency(float freq_)
{
	frequency = freq_;
	osc.setFrequency(frequency, sampleRate);
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

void Operator::setModOperator(Operator* modulator)
{
	modOperator = modulator;
}

float Operator::getNextSample()
{
	float currentEnv = env.getNextSample();
	float output = 0.f;
	//placeholder operator interaction
	if (modOperator != nullptr) { 
		// placeholder modulation depth of 100
		float modsample = modOperator->getNextSample();
	/*	DBG("modsample: " << modsample);
		DBG("Modulator: " << modOperator->opIndex);*/
		float current_freq = frequency;
		//DBG("Next freq: " << current_freq + modsample);
		setFrequency(current_freq + 10 * modsample);
	}
	output = osc.nextSample() * osc.amplitude * level * currentEnv;
	return output;
}

void Operator::updateRatio(float ratio_)
{
	ratio = ratio_;
}

void Operator::updateLevel(float level_)
{
	level = level_;
}

void Operator::noteOn(int note, int velocity)
{
	float freq = ratio * 440.0f * std::exp2(float(note - 69) / 12.0f); //this is the midi to freq formula
	setFrequency(freq);
	osc.amplitude = (velocity / 127.0f) * 0.5f;	
	env.noteOn();
}
void Operator::noteOff()
{
	env.noteOff();
}
