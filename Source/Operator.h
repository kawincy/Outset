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

enum class ModulationType
{
	FM,  // Frequency modulation (modulate Hz)
	PM   // Phase modulation (DX7-style, modulate phase angle)
};

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
	void resetRouting();
	// having multiple operators has to be handled carefully. We cannot call nextSample() multiple
	float getNextSample();
	void noteOn(int note, int velocity);
	void noteOff();
	void reset(float fs);
	void resetCache();
	void updateEnvParams(float attack, float decay, float sustain, float release);
	void updateRatio(float ratio_);
	void updateLevel(float level_);
	void updateTuning(float fine, float coarse);
	float getCachedSample();
	bool isCarrier() { return carrier; }
	void setCarrier(bool isCarrier) { carrier = isCarrier; }
	bool isFeedback() { return feedback; }
	void setFeedback(bool isFeedback) { feedback = isFeedback; }
	void setModulationType(ModulationType type) { modulationType = type; }
	ModulationType getModulationType() const { return modulationType; }
	void setModulationIndex(float index) { modulationIndex = index; }
	float getModulationIndex() const { return modulationIndex; }
	Oscillator osc;
	juce::ADSR env;
	void setCached() { cached = true; }
private:
	bool feedback = false; // feedback operator assignment
	bool carrier = false; // carrier operator assignment
	bool cached; // has the next sample been processed yet?
	bool inProgress = false; // re-entrancy guard to break cycles
	ModulationType modulationType = ModulationType::PM; // Default to DX7-style phase modulation
	float modulationIndex = 1.0f; // Modulation depth (FM index or PM index)
	float lastSample = 0.f;
	float cachedSample = 0.f;
	int opIndex;
	float sampleRate, baseFrequency, level, ratio, tuning, envValue, ampValue;
	int note;
	std::vector<Operator*> modOperators;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> freqSmooth; //multiplicative for frequency per juce docs
	juce::SmoothedValue<float> ampSmooth;
};
// dummy carrier inheritings from operator, overloads getNextSample to not modulate but average over all "modulators"
// 
// class DummyCarrier : public Operator
//{
