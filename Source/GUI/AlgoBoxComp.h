/*
  ==============================================================================

    AlgoBoxComp.h
    Created: 1 Apr 2025 2:26:29pm
    Author:  josep

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
class AlgoBoxComp : public juce::Component {
public:
    AlgoBoxComp();
    ~AlgoBoxComp() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void set_num(int num);
    void set_carrier(bool carrier);
    void set_existance(bool e);

private:
    std::string num_s;
    bool is_carrier;
    bool exists;

    juce::Colour mainBlue;
    juce::Colour accentBlue;
    juce::Colour offBlue;


};