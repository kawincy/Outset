/*
  ==============================================================================

    OutsetVerbUI.cpp
    
    User interface component implementation for Outset-Verb.

  ==============================================================================
*/

#include "OutsetVerbUI.h"

//==============================================================================
OutsetVerbUI::OutsetVerbUI(juce::AudioProcessorValueTreeState& apvtsRef)
    : apvts(apvtsRef)
{
    // Set up title label
    titleLabel.setText("Outset FX Rack", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
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
    apvts.addParameterListener("chainSlot1", this);
    apvts.addParameterListener("chainSlot2", this);
    apvts.addParameterListener("chainSlot3", this);
    apvts.addParameterListener("chainSlot4", this);
}

OutsetVerbUI::~OutsetVerbUI()
{
    // Remove parameter listeners
    apvts.removeParameterListener("chainSlot1", this);
    apvts.removeParameterListener("chainSlot2", this);
    apvts.removeParameterListener("chainSlot3", this);
    apvts.removeParameterListener("chainSlot4", this);
}

//==============================================================================
void OutsetVerbUI::paint(juce::Graphics& g)
{
    // Fill background with dark gradient
    juce::ColourGradient gradient(juce::Colours::darkslategrey, 0, 0,
                                  juce::Colours::black, 0, static_cast<float>(getHeight()), false);
    g.setGradientFill(gradient);
    g.fillAll();
}

void OutsetVerbUI::resized()
{
    // Safety check - don't layout if containers aren't created yet
    if (!bitCrusherContainer || !delayContainer || !eqContainer || !reverbContainer)
        return;
    
    auto bounds = getLocalBounds();
    
    // Position title at the top
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    
    // Position chain ordering UI below title
    auto chainBounds = bounds.removeFromTop(chainOrderingHeight);
    chainBounds.reduce(containerPadding, 5);
    
    // Improved spacing calculation with dedicated widths
    int inputLabelWidth = 80;
    int outputLabelWidth = 90;
    int arrowWidth = 25;
    int dropdownSpacing = 8;
    int totalFixedWidth = inputLabelWidth + outputLabelWidth + (3 * arrowWidth) + (7 * dropdownSpacing);
    int availableDropdownWidth = chainBounds.getWidth() - totalFixedWidth;
    int dropdownWidth = availableDropdownWidth / 4;
    
    // Layout with proper spacing
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
    
    // Add padding below chain ordering
    bounds.reduce(containerPadding, containerPadding);
    
    // Get current chain configuration
    std::array<int, 4> chainConfig = {0, 0, 0, 0};
    chainConfig[0] = static_cast<int>(apvts.getRawParameterValue("chainSlot1")->load());
    chainConfig[1] = static_cast<int>(apvts.getRawParameterValue("chainSlot2")->load());
    chainConfig[2] = static_cast<int>(apvts.getRawParameterValue("chainSlot3")->load());
    chainConfig[3] = static_cast<int>(apvts.getRawParameterValue("chainSlot4")->load());
    
    // Calculate container width for 4 slots
    int totalContainerPadding = containerPadding * 5;
    int containerWidth = (bounds.getWidth() - totalContainerPadding) / 4;
    
    // Position containers based on chain configuration
    for (int slotIndex = 0; slotIndex < 4; ++slotIndex)
    {
        int effectType = chainConfig[slotIndex];
        
        // Get the container bounds for this slot
        auto slotBounds = bounds.removeFromLeft(containerWidth);
        
        // Position the appropriate container in this slot
        EffectContainer* containerToPosition = nullptr;
        
        switch (effectType)
        {
            case 1: // Bit Crusher
                containerToPosition = bitCrusherContainer.get();
                break;
            case 2: // Delay
                containerToPosition = delayContainer.get();
                break;
            case 3: // EQ
                containerToPosition = eqContainer.get();
                break;
            case 4: // Reverb
                containerToPosition = reverbContainer.get();
                break;
            default: // None (0) or invalid
                break;
        }
        
        // Position the container if one was selected for this slot
        if (containerToPosition != nullptr)
        {
            containerToPosition->setBounds(slotBounds);
        }
        
        // Add spacing between slots
        bounds.removeFromLeft(containerPadding);
    }
}

juce::Rectangle<int> OutsetVerbUI::getRecommendedSize()
{
    return juce::Rectangle<int>(0, 0, windowWidth, windowHeight);
}

//==============================================================================
void OutsetVerbUI::setupEffectContainers()
{
    // Create BitCrusher container
    bitCrusherContainer = std::make_unique<EffectContainer>("Bit Crusher");
    bitCrusherContainer->addSlider("bitDepth", "Bit Depth", apvts);
    bitCrusherContainer->addSlider("sampleRateReduction", "Rate Reduction", apvts);
    bitCrusherContainer->addSlider("bitCrusherMix", "Mix", apvts);
    addAndMakeVisible(*bitCrusherContainer);
    
    // Create Delay container
    delayContainer = std::make_unique<EffectContainer>("Delay");
    delayContainer->addSlider("delayTime", "Time (ms)", apvts);
    delayContainer->addSlider("delayFeedback", "Feedback", apvts);
    delayContainer->addSlider("delayMix", "Mix", apvts);
    delayContainer->addSlider("delayLowPassCutoff", "LP Cutoff", apvts);
    addAndMakeVisible(*delayContainer);
    
    // Create EQ container with 2-column layout for better space utilization
    eqContainer = std::make_unique<EffectContainer>("Three Band EQ", EffectContainer::LayoutMode::TwoColumn);
    eqContainer->addSlider("lowGain", "Low Gain", apvts);
    eqContainer->addSlider("lowFreq", "Low Freq", apvts);
    eqContainer->addSlider("midGain", "Mid Gain", apvts);
    eqContainer->addSlider("midFreq", "Mid Freq", apvts);
    eqContainer->addSlider("midQ", "Mid Q", apvts);
    eqContainer->addSlider("highGain", "High Gain", apvts);
    eqContainer->addSlider("highFreq", "High Freq", apvts);
    addAndMakeVisible(*eqContainer);
    
    // Create Reverb container
    reverbContainer = std::make_unique<EffectContainer>("Reverb");
    reverbContainer->addSlider("roomSize", "Room Size", apvts);
    reverbContainer->addSlider("damping", "Damping", apvts);
    reverbContainer->addSlider("reverbMix", "Mix", apvts);
    reverbContainer->addSlider("width", "Width", apvts);
    reverbContainer->addToggleButton("freezeMode", "Freeze", apvts);
    addAndMakeVisible(*reverbContainer);
}

void OutsetVerbUI::setupChainOrderingUI()
{
    // Setup audio input label
    audioInputLabel.setText("Audio Input", juce::dontSendNotification);
    audioInputLabel.setFont(juce::Font(14.0f));
    audioInputLabel.setJustificationType(juce::Justification::centred);
    audioInputLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(audioInputLabel);
    
    // Setup flow arrows
    for (int i = 0; i < 3; ++i)
    {
        flowArrows[i].setText("->", juce::dontSendNotification);
        flowArrows[i].setFont(juce::Font(16.0f, juce::Font::bold));
        flowArrows[i].setJustificationType(juce::Justification::centred);
        flowArrows[i].setColour(juce::Label::textColourId, juce::Colours::lightblue);
        addAndMakeVisible(flowArrows[i]);
    }
    
    // Setup audio output label
    audioOutputLabel.setText("Audio Output", juce::dontSendNotification);
    audioOutputLabel.setFont(juce::Font(14.0f));
    audioOutputLabel.setJustificationType(juce::Justification::centred);
    audioOutputLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(audioOutputLabel);
    
    // Setup chain dropdowns and their attachments
    const juce::StringArray effectOptions = {"None", "Bit Crusher", "Delay", "EQ", "Reverb"};
    
    for (int i = 0; i < 4; ++i)
    {
        // Create dropdown
        chainDropdowns[i] = std::make_unique<juce::ComboBox>();
        chainDropdowns[i]->addItemList(effectOptions, 1);  // Start IDs from 1
        chainDropdowns[i]->setSelectedId(1);  // Default to "None" (ID 1)
        addAndMakeVisible(*chainDropdowns[i]);
        
        // Create attachment
        juce::String paramID = "chainSlot" + juce::String(i + 1);
        chainAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts, paramID, *chainDropdowns[i]);
    }
}

void OutsetVerbUI::updateEffectContainerStates()
{
    // Get current chain configuration
    std::array<int, 4> chainConfig;
    chainConfig[0] = static_cast<int>(apvts.getRawParameterValue("chainSlot1")->load());
    chainConfig[1] = static_cast<int>(apvts.getRawParameterValue("chainSlot2")->load());
    chainConfig[2] = static_cast<int>(apvts.getRawParameterValue("chainSlot3")->load());
    chainConfig[3] = static_cast<int>(apvts.getRawParameterValue("chainSlot4")->load());
    
    // Check which effects are currently in the chain
    bool bitCrusherInChain = false;
    bool delayInChain = false;
    bool eqInChain = false;
    bool reverbInChain = false;
    
    for (int config : chainConfig)
    {
        switch (config)
        {
            case 1: bitCrusherInChain = true; break;  // Bit Crusher
            case 2: delayInChain = true; break;       // Delay
            case 3: eqInChain = true; break;          // EQ
            case 4: reverbInChain = true; break;      // Reverb
            default: break;  // None or invalid
        }
    }
    
    // Update container states AND visibility
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
    
    // Trigger layout update to reposition visible containers
    resized();
}

void OutsetVerbUI::updateChainDropdownOptions()
{
    // Get current chain configuration
    std::array<int, 4> chainConfig;
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramID = "chainSlot" + juce::String(i + 1);
        chainConfig[i] = static_cast<int>(apvts.getRawParameterValue(paramID)->load());
    }
    
    // Update each dropdown
    for (int dropdownIndex = 0; dropdownIndex < 4; ++dropdownIndex)
    {
        if (!chainDropdowns[dropdownIndex])
            continue;
        
        // Effect IDs: 1=None, 2=BitCrusher, 3=Delay, 4=EQ, 5=Reverb
        // Check which effects are used in OTHER slots
        for (int effectID = 2; effectID <= 5; ++effectID)  // Skip "None" (ID 1)
        {
            bool isUsedInOtherSlot = false;
            
            // Check all OTHER slots for this effect
            for (int otherSlot = 0; otherSlot < 4; ++otherSlot)
            {
                if (otherSlot != dropdownIndex && chainConfig[otherSlot] + 1 == effectID)
                {
                    isUsedInOtherSlot = true;
                    break;
                }
            }
            
            // Enable/disable the item in the current dropdown
            chainDropdowns[dropdownIndex]->setItemEnabled(effectID, !isUsedInOtherSlot);
        }
        
        // "None" (ID 1) should always be enabled
        chainDropdowns[dropdownIndex]->setItemEnabled(1, true);
    }
}

void OutsetVerbUI::parameterChanged(const juce::String& parameterID, float newValue)
{
    // Update effect container states when chain configuration changes
    if (parameterID.startsWith("chainSlot"))
    {
        updateChainDropdownOptions();  // Update dropdown options first
        updateEffectContainerStates();
    }
}
