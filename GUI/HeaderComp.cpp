/*
  ==============================================================================
    HeaderComp.cpp
  ==============================================================================
*/

#include <JuceHeader.h>
#include "Colors.h"
#include "HeaderComp.h"

//==============================================================================
// A transparent full-screen catcher that dismisses the popup when
// the user clicks anywhere outside it.
class PopupDismisser : public juce::Component
{
public:
    std::function<void()> onClickOutside;

    PopupDismisser()
    {
        setInterceptsMouseClicks(true, true);
        setAlwaysOnTop(true);
    }

    void mouseDown(const juce::MouseEvent&) override
    {
        if (onClickOutside) onClickOutside();
    }
};

//==============================================================================
HeaderComp::HeaderComp(PresetManager& pm)
    : presetPanel(pm)
{
    addAndMakeVisible(presetPanel);

    fxButton.setButtonText("FX");
    fxButton.setColour(juce::TextButton::buttonColourId,   colors().bg);
    fxButton.setColour(juce::TextButton::buttonOnColourId, colors().accent);
    fxButton.setColour(juce::TextButton::textColourOffId,  colors().white);
    fxButton.setColour(juce::TextButton::textColourOnId,   colors().white);
    fxButton.onClick = [this] { showFXPopup(); };
    addAndMakeVisible(fxButton);

    themeButton.setButtonText(juce::CharPointer_UTF8("\xf0\x9f\x8e\xa8")); // 🎨 or just use text
    themeButton.setButtonText("Theme");
    themeButton.setColour(juce::TextButton::buttonColourId,   colors().bg);
    themeButton.setColour(juce::TextButton::buttonOnColourId, colors().accent);
    themeButton.setColour(juce::TextButton::textColourOffId,  colors().main);
    themeButton.setColour(juce::TextButton::textColourOnId,   colors().white);
    themeButton.setClickingTogglesState(true);
    themeButton.onClick = [this]
    {
        if (themeButton.getToggleState())
            showThemePopup();
        else
            hideThemePopup();
    };
    addAndMakeVisible(themeButton);
}

HeaderComp::~HeaderComp()
{
    hideThemePopup();
}

//==============================================================================
void HeaderComp::paint(juce::Graphics& g)
{
    g.fillAll(colors().bg);
    g.setColour(colors().main);
    g.setFont(juce::FontOptions(14.0f));
    g.drawText("Presets", getLocalBounds(), juce::Justification::centred, true);
}

void HeaderComp::resized()
{
    auto bounds = getBounds();
    int  width  = bounds.getWidth();
    int  height = bounds.getHeight();

    // Preset panel takes the left 2/3
    presetPanel.setBounds(bounds.withWidth(width * 2 / 3));

    // Right third split between Theme button and FX button
    auto rightArea = juce::Rectangle<int>(width * 2 / 3, bounds.getY(),
                                          width / 3, height).reduced(6);
    int btnW = (rightArea.getWidth() - 6) / 2;

    themeButton.setBounds(rightArea.getX(), rightArea.getY(),
                          btnW, rightArea.getHeight());

    fxButton.setBounds(rightArea.getX() + btnW + 6, rightArea.getY(),
                       btnW, rightArea.getHeight());
}

//==============================================================================
void HeaderComp::showThemePopup()
{
    if (themePopup) return;

    // Wrapper component that holds the ThemePicker and a dismiss layer
    auto* wrapper = new juce::Component();
    themePopup.reset(wrapper);

    // Build the picker inside the wrapper
    auto* picker = new ThemePicker();
    picker->onThemeChanged = [this]()
    {
        // Refresh our own button colours
        fxButton.setColour(juce::TextButton::buttonColourId,  colors().bg);
        fxButton.setColour(juce::TextButton::textColourOffId, colors().white);
        themeButton.setColour(juce::TextButton::buttonColourId,  colors().bg);
        themeButton.setColour(juce::TextButton::textColourOffId, colors().main);
        repaint();

        if (onThemeChanged) onThemeChanged();
    };

    const int pickerW = 220;
    const int pickerH = 44;

    wrapper->addAndMakeVisible(picker);
    picker->setBounds(0, 0, pickerW, pickerH);
    wrapper->setSize(pickerW, pickerH);

    // Position the popup just below the theme button, in top-level coordinates
    auto btnScreenBounds = themeButton.getBounds()
                               .translated(getScreenX() - getX(),
                                           getScreenY() - getY());

    // Add to top-level component (the editor) so it floats above everything
    if (auto* topLevel = getTopLevelComponent())
    {
        auto btnInTop = topLevel->getLocalArea(this, themeButton.getBounds());
        wrapper->setBounds(btnInTop.getX(),
                           btnInTop.getBottom() + 2,
                           pickerW, pickerH);
        topLevel->addAndMakeVisible(wrapper);
        wrapper->toFront(false);
    }
}

void HeaderComp::hideThemePopup()
{
    if (!themePopup) return;

    if (auto* topLevel = getTopLevelComponent())
        topLevel->removeChildComponent(themePopup.get());

    themePopup.reset();
    themeButton.setToggleState(false, juce::dontSendNotification);
}

void HeaderComp::showFXPopup()
{
    if (onFXButtonClicked) onFXButtonClicked();
    presetPanel.setBounds(getBounds());
}