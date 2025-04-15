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
	void setBaseFrequency(float freq);

	void setLevel(float amplitude);
	void addModOperator(Operator* modulator);
	// having multiple operators has to be handled carefully. We cannot call nextSample() multiple
	float getNextSample();
	void noteOn(int note, int velocity);
	void noteOff();
	void reset(float fs);
	void resetCache();
	void updateEnvParams(float attack, float decay, float sustain, float release);
	void updateRatio(float ratio_);
	void updateLevel(float level_);
	float getCachedSample();
	bool isCarrier() { return carrier; }
	void setCarrier(bool isCarrier) { carrier = isCarrier; }
	bool isFeedback() { return feedback; }
	void setFeedback(bool isFeedback) { feedback = isFeedback; }
	Oscillator osc;
	juce::ADSR env;

private:
	bool feedback = false; // feedback operator assignment
	bool carrier = false; // carrier operator assignment
	bool cached; // has the next sample been processed yet?
	float lastSample = 0.f;
	float cachedSample;
	int opIndex;
	float sampleRate, frequency, level, ratio;
	std::vector<Operator*> modOperators;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> freqSmooth; //multiplicative for frequency per juce docs
	juce::SmoothedValue<float> ampSmooth;
};
// dummy carrier inheritings from operator, overloads getNextSample to not modulate but average over all "modulators"
// 
// class DummyCarrier : public Operator
//{
