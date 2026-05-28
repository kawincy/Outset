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
FXComp::FXComp(juce::AudioProcessorValueTreeState& apvtsRef, OutsetVerbEngine& fxEngine)
    : apvtsRef(apvtsRef), fxEngine(fxEngine)
{
    titleLabel.setText("FX Rack", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, colors().main);
    addAndMakeVisible(titleLabel);

    setupChainOrderingUI();
    setupEffectContainers();
    setupBypassButtons();
    updateChainDropdownOptions();
    updateEffectContainerStates();

    apvtsRef.addParameterListener("chainSlot1", this);
    apvtsRef.addParameterListener("chainSlot2", this);
    apvtsRef.addParameterListener("chainSlot3", this);
    apvtsRef.addParameterListener("chainSlot4", this);
}

FXComp::~FXComp()
{
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
    if (!bitCrusherContainer || !delayContainer || !reverbContainer || !eqEditor)
        return;

    auto bounds = getLocalBounds();

    titleLabel.setBounds(bounds.removeFromTop(titleHeight));

    auto chainBounds = bounds.removeFromTop(chainOrderingHeight);
    chainBounds.reduce(containerPadding, 5);

    int inputLabelWidth   = 70;
    int outputLabelWidth  = 80;
    int arrowWidth        = 20;
    int dropdownSpacing   = 6;
    int totalFixedWidth   = inputLabelWidth + outputLabelWidth + (3 * arrowWidth) + (7 * dropdownSpacing);
    int availableDropdown = chainBounds.getWidth() - totalFixedWidth;
    int dropdownWidth     = availableDropdown / 4;

    audioInputLabel.setBounds(chainBounds.removeFromLeft(inputLabelWidth));
    chainBounds.removeFromLeft(dropdownSpacing);

    for (int i = 0; i < 4; ++i)
    {
        if (i > 0)
        {
            flowArrows[i - 1].setBounds(chainBounds.removeFromLeft(arrowWidth));
            chainBounds.removeFromLeft(dropdownSpacing);
        }
        chainDropdowns[i]->setBounds(chainBounds.removeFromLeft(dropdownWidth));
        if (i < 3) chainBounds.removeFromLeft(dropdownSpacing);
    }

    chainBounds.removeFromLeft(dropdownSpacing);
    audioOutputLabel.setBounds(chainBounds.removeFromLeft(outputLabelWidth));

    bounds.reduce(containerPadding, containerPadding);

    std::array<int, 4> chainConfig = { 0, 0, 0, 0 };
    chainConfig[0] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot1")->load());
    chainConfig[1] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot2")->load());
    chainConfig[2] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot3")->load());
    chainConfig[3] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot4")->load());

    const int totalPad   = containerPadding * 5;
    const int totalAvail = bounds.getWidth() - totalPad;
    const int slotWidth  = totalAvail / 4;

    // Reserve space at the bottom of each slot for the bypass button
    auto bypassStrip = bounds;
    auto contentArea = bounds;
    auto bypassRow   = contentArea.removeFromBottom(bypassButtonHeight + 4);

    for (int slotIndex = 0; slotIndex < 4; ++slotIndex)
    {
        int effectType    = chainConfig[slotIndex];
        auto slotContent  = contentArea.removeFromLeft(slotWidth);
        auto slotBypass   = bypassRow.removeFromLeft(slotWidth);

        // Position bypass button centred at the bottom of this slot
        if (bypassButtons[slotIndex])
        {
            bool hasEffect = (effectType != 0);
            bypassButtons[slotIndex]->setVisible(hasEffect);
            bypassButtons[slotIndex]->setBounds(
                slotBypass.withSizeKeepingCentre(slotWidth - 8, bypassButtonHeight));
        }

        switch (effectType)
        {
            case 1: bitCrusherContainer->setBounds(slotContent); break;
            case 2: delayContainer->setBounds(slotContent);      break;
            case 3: eqEditor->setBounds(slotContent);            break;
            case 4: reverbContainer->setBounds(slotContent);     break;
            default: break;
        }

        contentArea.removeFromLeft(containerPadding);
        bypassRow.removeFromLeft(containerPadding);
    }
}

//==============================================================================
void FXComp::setupEffectContainers()
{
    bitCrusherContainer = std::make_unique<EffectContainer>("Bit Crusher");
    bitCrusherContainer->addSlider("bitDepth",            "Bit Depth",      apvtsRef);
    bitCrusherContainer->addSlider("sampleRateReduction", "Rate Reduction",  apvtsRef);
    bitCrusherContainer->addSlider("bitCrusherMix",       "Mix",             apvtsRef);
    addAndMakeVisible(*bitCrusherContainer);

    delayContainer = std::make_unique<EffectContainer>("Delay");
    delayContainer->addSlider("delayTime",          "Time (ms)", apvtsRef);
    delayContainer->addSlider("delayFeedback",      "Feedback",  apvtsRef);
    delayContainer->addSlider("delayMix",           "Mix",       apvtsRef);
    delayContainer->addSlider("delayLowPassCutoff", "LP Cutoff", apvtsRef);
    addAndMakeVisible(*delayContainer);

    eqEditor = std::make_unique<ThreeBandEQEditor>(fxEngine.getEQNode(), apvtsRef);
    addAndMakeVisible(*eqEditor);

    eqEditor->onExpandToggled = [this](bool isExpanded)
    {
        if (onEQExpandToggled) onEQExpandToggled(isExpanded);
    };

    reverbContainer = std::make_unique<EffectContainer>("Reverb");
    reverbContainer->addSlider("roomSize",  "Room Size", apvtsRef);
    reverbContainer->addSlider("damping",   "Damping",   apvtsRef);
    reverbContainer->addSlider("reverbMix", "Mix",       apvtsRef);
    reverbContainer->addSlider("width",     "Width",     apvtsRef);
    reverbContainer->addToggleButton("freezeMode", "Freeze", apvtsRef);
    addAndMakeVisible(*reverbContainer);
}

void FXComp::setupBypassButtons()
{
    for (int i = 0; i < 4; ++i)
    {
        bypassButtons[i] = std::make_unique<juce::ToggleButton>("Bypass");
        bypassButtons[i]->setColour(juce::ToggleButton::textColourId,   colors().main);
        bypassButtons[i]->setColour(juce::ToggleButton::tickColourId,   colors().accent);
        bypassButtons[i]->setColour(juce::ToggleButton::tickDisabledColourId, colors().accent.withAlpha(0.4f));
        bypassButtons[i]->setVisible(false); // hidden until a slot is occupied

        const int slotIndex = i;
        bypassButtons[i]->onClick = [this, slotIndex]()
        {
            slotBypassed[slotIndex] = bypassButtons[slotIndex]->getToggleState();
            applyBypassState(slotIndex, slotBypassed[slotIndex]);
        };

        addAndMakeVisible(*bypassButtons[i]);
    }
}

void FXComp::applyBypassState(int slotIndex, bool bypassed)
{
    int effectType = static_cast<int>(
        apvtsRef.getRawParameterValue("chainSlot" + juce::String(slotIndex + 1))->load());

    float dimAlpha = bypassed ? 0.4f : 1.0f;

    switch (effectType)
    {
        case 1:
            bitCrusherContainer->setAlpha(dimAlpha);
            bitCrusherContainer->setEnabled(!bypassed);
            break;
        case 2:
            delayContainer->setAlpha(dimAlpha);
            delayContainer->setEnabled(!bypassed);
            break;
        case 3:
            eqEditor->setAlpha(dimAlpha);
            eqEditor->setEnabled(!bypassed);
            break;
        case 4:
            reverbContainer->setAlpha(dimAlpha);
            reverbContainer->setEnabled(!bypassed);
            break;
        default:
            break;
    }

    // Tell the engine — you'll wire this into OutsetVerbEngine in the next step
    // fxEngine.setSlotBypassed(slotIndex, bypassed);
}

void FXComp::setupChainOrderingUI()
{
    audioInputLabel.setText("Input", juce::dontSendNotification);
    audioInputLabel.setFont(juce::Font(12.0f));
    audioInputLabel.setJustificationType(juce::Justification::centred);
    audioInputLabel.setColour(juce::Label::textColourId, colors().main);
    addAndMakeVisible(audioInputLabel);

    for (int i = 0; i < 3; ++i)
    {
        flowArrows[i].setText("->", juce::dontSendNotification);
        flowArrows[i].setFont(juce::Font(14.0f, juce::Font::bold));
        flowArrows[i].setJustificationType(juce::Justification::centred);
        flowArrows[i].setColour(juce::Label::textColourId, colors().accent);
        addAndMakeVisible(flowArrows[i]);
    }

    audioOutputLabel.setText("Output", juce::dontSendNotification);
    audioOutputLabel.setFont(juce::Font(12.0f));
    audioOutputLabel.setJustificationType(juce::Justification::centred);
    audioOutputLabel.setColour(juce::Label::textColourId, colors().main);
    addAndMakeVisible(audioOutputLabel);

    const juce::StringArray effectOptions = { "None", "Bit Crusher", "Delay", "EQ", "Reverb" };

    for (int i = 0; i < 4; ++i)
    {
        chainDropdowns[i] = std::make_unique<juce::ComboBox>();
        chainDropdowns[i]->addItemList(effectOptions, 1);
        chainDropdowns[i]->setSelectedId(1);
        addAndMakeVisible(*chainDropdowns[i]);

        juce::String paramID = "chainSlot" + juce::String(i + 1);
        chainAttachments[i]  = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvtsRef, paramID, *chainDropdowns[i]);
    }
}

void FXComp::updateEffectContainerStates()
{
    std::array<int, 4> chainConfig;
    chainConfig[0] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot1")->load());
    chainConfig[1] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot2")->load());
    chainConfig[2] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot3")->load());
    chainConfig[3] = static_cast<int>(apvtsRef.getRawParameterValue("chainSlot4")->load());

    bool bitCrusherInChain = false, delayInChain = false,
         eqInChain = false,         reverbInChain = false;

    for (int config : chainConfig)
    {
        switch (config)
        {
            case 1: bitCrusherInChain = true; break;
            case 2: delayInChain      = true; break;
            case 3: eqInChain         = true; break;
            case 4: reverbInChain     = true; break;
            default: break;
        }
    }

    if (bitCrusherContainer) { bitCrusherContainer->setEnabledState(bitCrusherInChain); bitCrusherContainer->setVisible(bitCrusherInChain); }
    if (delayContainer)      { delayContainer->setEnabledState(delayInChain);           delayContainer->setVisible(delayInChain); }
    if (reverbContainer)     { reverbContainer->setEnabledState(reverbInChain);         reverbContainer->setVisible(reverbInChain); }
    if (eqEditor)
    {
        eqEditor->setVisible(eqInChain);
        if (!eqInChain && onEQExpandToggled) onEQExpandToggled(false);
    }

    // Reset bypass state for slots that are now empty
    for (int i = 0; i < 4; ++i)
    {
        if (chainConfig[i] == 0 && slotBypassed[i])
        {
            slotBypassed[i] = false;
            if (bypassButtons[i]) bypassButtons[i]->setToggleState(false, juce::dontSendNotification);
        }
    }

    resized();
}

void FXComp::updateChainDropdownOptions()
{
    std::array<int, 4> chainConfig;
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramID = "chainSlot" + juce::String(i + 1);
        chainConfig[i] = static_cast<int>(apvtsRef.getRawParameterValue(paramID)->load());
    }

    for (int dropdownIndex = 0; dropdownIndex < 4; ++dropdownIndex)
    {
        if (!chainDropdowns[dropdownIndex]) continue;

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

void FXComp::parameterChanged(const juce::String& parameterID, float /*newValue*/)
{
    if (parameterID.startsWith("chainSlot"))
    {
        updateChainDropdownOptions();
        updateEffectContainerStates();
    }
}