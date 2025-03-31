/*
  ==============================================================================

    operator.h
    Created: 31 Mar 2025 1:30:40pm
    Author:  ryanb

  ==============================================================================
*/

#pragma once
#include "DSP/Oscillator.h"
#include <JuceHeader.h>

class Operator
{
public:
	Operator();
	Operator(int index);
	~Operator();
	void init(int opIndex_);
	void setFrequency(float freq);
	void setLevel(float amplitude);
	void setModOperator(Operator* modulator);
	float getNextSample();
	void noteOn(int note, int velocity);
	void noteOff();
	void reset(float fs);
	void updateEnvParams(float attack, float decay, float sustain, float release);
	void updateRatio(float ratio_);
	void updateLevel(float level_);
private:
	int opIndex;
	float sampleRate, frequency, level, ratio;
	Operator* modOperator = nullptr;
	Oscillator osc;
	juce::ADSR env;
};