/*
  ==============================================================================

    FXComp.cpp
    Created: 13 Nov 2025

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FXComp.h"
#include "Colors.h"

//==============================================================================
FXComp::FXComp(juce::AudioProcessorValueTreeState& apvtsRef)
    : apvtsRef(apvtsRef)
{
    // Set up title label
    titleLabel.setText("FX Rack", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, colors().main);
    addAndMakeVisible(titleLabel);
    
    // Setup chain ordering UI
    setupChainOrderingUI();
    
    // Setup all effect containers
    setupEffectContainers();
    
    // Update dropdown options based on initial chain configuration
    updateChainDropdownOptions();
    
    // Update effect container states based on initial chain configuration
    updateEffectContainerStates();
    
    // Add parameter listeners for chain configuration changes
    apvtsRef.addParameterListener("chainSlot1", this);
    apvtsRef.addParameterListener("chainSlot2", this);
    apvtsRef.addParameterListener("chainSlot3", this);
    apvtsRef.addParameterListener("chainSlot4", this);
}

FXComp::~FXComp()
{
    // Remove parameter listeners
    apvtsRef.removeParameterListener("chainSlot1", this);
    apvtsRef.removeParameterListener("chainSlot2", this);
    apvtsRef.removeParameterListener("chainSlot3", this);
    apvtsRef.removeParameterListener("chainSlot4", this);
}

//==============================================================================
void FXComp::paint(juce::Graphics& g)
{
    g.fillAll(colors().bg);
    g.setColour(colors().accent);
    g.drawRect(getLocalBounds(), 1);
}

void FXComp::resized()
{
    // Safety check
    if (!bitCrusherContainer || !delayContainer || !eqContainer || !reverbContainer)
        return;
    
    auto bounds = getLocalBounds();
    
    // Position title at the top
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    
    // Position chain ordering UI below title
    auto chainBounds = bounds.removeFromTop(chainOrderingHeight);
    chainBounds.reduce(containerPadding, 5);
    
    // Calculate layout widths
    int inputLabelWidth = 70;
    int outputLabelWidth = 80;
    int arrowWidth = 20;
    int dropdownSpacing = 6;
    int totalFixedWidth = inputLabelWidth + outputLabelWidth + (3 * arrowWidth) + (7 * dropdownSpacing);
    int availableDropdownWidth = chainBounds.getWidth() - totalFixedWidth;
    int dropdownWidth = availableDropdownWidth / 4;
    
    // Layout chain UI
    audioInputLabel.setBounds(chainBounds.removeFromLeft(inputLabelWidth));
    chainBounds.removeFromLeft(dropdownSpacing);
    
    for (int i = 0; i < 4; ++i)
    {
        if (i > 0)
        {
            flowArrows[i-1].setBounds(chainBounds.removeFromLeft(arrowWidth));
            chainBounds.removeFromLeft(dropdownSpacing);
        }
        
        chainDropdowns[i]->setBounds(chainBounds.removeFromLeft(dropdownWidth));
        if (i < 3) chainBounds.removeFromLeft(dropdownSpacing);
    }
    
    chainBounds.removeFromLeft(dropdownSpacing);
    audioOutputLabel.setBounds(chainBounds.removeFromLeft(outputLabelWidth));
    
    // Add padding for effect containers
    bounds.reduce(containerPadding, containerPadding);
    
    // Get current chain configuration
    std::array<int, 4> chainConfig = {0, 0, 0, 0};
    chainConfig[0] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot1")->load());
    chainConfig[1] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot2")->load());
    chainConfig[2] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot3")->load());
    chainConfig[3] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot4")->load());
    
    // Calculate container width for 4 slots
    int totalContainerPadding = containerPadding * 5;
    int containerWidth = (bounds.getWidth() - totalContainerPadding) / 4;
    
    // Position containers based on chain configuration
    for (int slotIndex = 0; slotIndex < 4; ++slotIndex)
    {
        int effectType = chainConfig[slotIndex];
        auto slotBounds = bounds.removeFromLeft(containerWidth);
        
        EffectContainer* containerToPosition = nullptr;
        
        switch (effectType)
        {
            case 1: containerToPosition = bitCrusherContainer.get(); break;
            case 2: containerToPosition = delayContainer.get(); break;
            case 3: containerToPosition = eqContainer.get(); break;
            case 4: containerToPosition = reverbContainer.get(); break;
            default: break;
        }
        
        if (containerToPosition != nullptr)
            containerToPosition->setBounds(slotBounds);
        
        bounds.removeFromLeft(containerPadding);
    }
}

//==============================================================================
void FXComp::setupEffectContainers()
{
    // Create BitCrusher container
    bitCrusherContainer = std::make_unique<EffectContainer>("Bit Crusher");
    bitCrusherContainer->addSlider("bitDepth", "Bit Depth", apvtsRef);
    bitCrusherContainer->addSlider("sampleRateReduction", "Rate Reduction", apvtsRef);
    bitCrusherContainer->addSlider("bitCrusherMix", "Mix", apvtsRef);
    addAndMakeVisible(*bitCrusherContainer);
    
    // Create Delay container
    delayContainer = std::make_unique<EffectContainer>("Delay");
    delayContainer->addSlider("delayTime", "Time (ms)", apvtsRef);
    delayContainer->addSlider("delayFeedback", "Feedback", apvtsRef);
    delayContainer->addSlider("delayMix", "Mix", apvtsRef);
    delayContainer->addSlider("delayLowPassCutoff", "LP Cutoff", apvtsRef);
    addAndMakeVisible(*delayContainer);
    
    // Create EQ container
    eqContainer = std::make_unique<EffectContainer>("Three Band EQ", EffectContainer::LayoutMode::TwoColumn);
    eqContainer->addSlider("lowGain", "Low Gain", apvtsRef);
    eqContainer->addSlider("lowFreq", "Low Freq", apvtsRef);
    eqContainer->addSlider("midGain", "Mid Gain", apvtsRef);
    eqContainer->addSlider("midFreq", "Mid Freq", apvtsRef);
    eqContainer->addSlider("midQ", "Mid Q", apvtsRef);
    eqContainer->addSlider("highGain", "High Gain", apvtsRef);
    eqContainer->addSlider("highFreq", "High Freq", apvtsRef);
    addAndMakeVisible(*eqContainer);
    
    // Create Reverb container
    reverbContainer = std::make_unique<EffectContainer>("Reverb");
    reverbContainer->addSlider("roomSize", "Room Size", apvtsRef);
    reverbContainer->addSlider("damping", "Damping", apvtsRef);
    reverbContainer->addSlider("reverbMix", "Mix", apvtsRef);
    reverbContainer->addSlider("width", "Width", apvtsRef);
    reverbContainer->addToggleButton("freezeMode", "Freeze", apvtsRef);
    addAndMakeVisible(*reverbContainer);
}

void FXComp::setupChainOrderingUI()
{
    // Setup audio input label
    audioInputLabel.setText("Input", juce::dontSendNotification);
    audioInputLabel.setFont(juce::Font(12.0f));
    audioInputLabel.setJustificationType(juce::Justification::centred);
    audioInputLabel.setColour(juce::Label::textColourId, colors().main);
    addAndMakeVisible(audioInputLabel);
    
    // Setup flow arrows
    for (int i = 0; i < 3; ++i)
    {
        flowArrows[i].setText("->", juce::dontSendNotification);
        flowArrows[i].setFont(juce::Font(14.0f, juce::Font::bold));
        flowArrows[i].setJustificationType(juce::Justification::centred);
        flowArrows[i].setColour(juce::Label::textColourId, colors().accent);
        addAndMakeVisible(flowArrows[i]);
    }
    
    // Setup audio output label
    audioOutputLabel.setText("Output", juce::dontSendNotification);
    audioOutputLabel.setFont(juce::Font(12.0f));
    audioOutputLabel.setJustificationType(juce::Justification::centred);
    audioOutputLabel.setColour(juce::Label::textColourId, colors().main);
    addAndMakeVisible(audioOutputLabel);
    
    // Setup chain dropdowns
    const juce::StringArray effectOptions = {"None", "Bit Crusher", "Delay", "EQ", "Reverb"};
    
    for (int i = 0; i < 4; ++i)
    {
        chainDropdowns[i] = std::make_unique<juce::ComboBox>();
        chainDropdowns[i]->addItemList(effectOptions, 1);
        chainDropdowns[i]->setSelectedId(1);
        addAndMakeVisible(*chainDropdowns[i]);
        
        juce::String paramID = "chainSlot" + juce::String(i + 1);
        chainAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvtsRef, paramID, *chainDropdowns[i]);
    }
}

void FXComp::updateEffectContainerStates()
{
    // Get current chain configuration
    std::array<int, 4> chainConfig;
    chainConfig[0] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot1")->load());
    chainConfig[1] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot2")->load());
    chainConfig[2] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot3")->load());
    chainConfig[3] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot4")->load());
    
    // Check which effects are in the chain
    bool bitCrusherInChain = false;
    bool delayInChain = false;
    bool eqInChain = false;
    bool reverbInChain = false;
    
    for (int config : chainConfig)
    {
        switch (config)
        {
            case 1: bitCrusherInChain = true; break;
            case 2: delayInChain = true; break;
            case 3: eqInChain = true; break;
            case 4: reverbInChain = true; break;
            default: break;
        }
    }
    
    // Update container states and visibility
    if (bitCrusherContainer)
    {
        bitCrusherContainer->setEnabledState(bitCrusherInChain);
        bitCrusherContainer->setVisible(bitCrusherInChain);
    }
    
    if (delayContainer)
    {
        delayContainer->setEnabledState(delayInChain);
        delayContainer->setVisible(delayInChain);
    }
    
    if (eqContainer)
    {
        eqContainer->setEnabledState(eqInChain);
        eqContainer->setVisible(eqInChain);
    }
    
    if (reverbContainer)
    {
        reverbContainer->setEnabledState(reverbInChain);
        reverbContainer->setVisible(reverbInChain);
    }
    
    resized();
}

void FXComp::updateChainDropdownOptions()
{
    // Get current chain configuration
    std::array<int, 4> chainConfig;
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramID = "chainSlot" + juce::String(i + 1);
        chainConfig[i] = static_cast<int>(apvtsRef.getRawParameterValue(paramID)->load());
    }
    
    // Update each dropdown
    for (int dropdownIndex = 0; dropdownIndex < 4; ++dropdownIndex)
    {
        if (!chainDropdowns[dropdownIndex])
            continue;
        
        // Effect IDs: 1=None, 2=BitCrusher, 3=Delay, 4=EQ, 5=Reverb
        for (int effectID = 2; effectID <= 5; ++effectID)
        {
            bool isUsedInOtherSlot = false;
            
            for (int otherSlot = 0; otherSlot < 4; ++otherSlot)
            {
                if (otherSlot != dropdownIndex && chainConfig[otherSlot] + 1 == effectID)
                {
                    isUsedInOtherSlot = true;
                    break;
                }
            }
            
            chainDropdowns[dropdownIndex]->setItemEnabled(effectID, !isUsedInOtherSlot);
        }
        
        chainDropdowns[dropdownIndex]->setItemEnabled(1, true);
    }
}

void FXComp::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID.startsWith("chainSlot"))
    {
        updateChainDropdownOptions();
        updateEffectContainerStates();
    }
}
