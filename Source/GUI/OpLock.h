/*
  ==============================================================================

    OpLock.h
    Created: 7 Sep 2025 4:59:46pm
    Author:  ryanb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

class OpLock : public juce::Component
{
public:
    OpLock()
    {
        loadDrawables();
        button = std::make_unique<juce::DrawableButton>("opLock", juce::DrawableButton::ImageRaw);
        updateButtonImages();
        button->onClick = [this]() { toggle(); };
        addAndMakeVisible(button.get());
    }

    // external callback
    std::function<void(bool)> onToggle;

    bool isLocked() const noexcept { return locked; }
    void setLocked(bool shouldBeLocked)
    {
        if (locked != shouldBeLocked)
        {
            locked = shouldBeLocked;
            updateButtonImages();
            if (onToggle) onToggle(locked);
        }
    }

    void resized() override
    {
        if (button)
            button->setBounds(getLocalBounds());
    }

private:
    void toggle()
    {
        locked = !locked;
        updateButtonImages();
        if (onToggle) onToggle(locked);
    }

    static std::unique_ptr<juce::Drawable> loadSVGFromMemory(const void* data, int size)
    {
        if (data == nullptr || size <= 0) return {};
        auto svgString = juce::String::fromUTF8(static_cast<const char*>(data), size);
        auto xml = juce::XmlDocument::parse(svgString);
        if (!xml) return {};
        return juce::Drawable::createFromSVG(*xml);
    }

    void loadDrawables()
    {
        lockedDrawable = loadSVGFromMemory(BinaryData::locked_svg, BinaryData::locked_svgSize);
        unlockedDrawable = loadSVGFromMemory(BinaryData::unlocked_svg, BinaryData::unlocked_svgSize);
    }

    void updateButtonImages()
    {
        if (!button)
            return;

        juce::Drawable* normal = locked ? lockedDrawable.get() : unlockedDrawable.get();

        // Clone for over/down states to allow tinting if desired later
        std::unique_ptr<juce::Drawable> over(normal ? normal->createCopy() : nullptr);
        std::unique_ptr<juce::Drawable> down(normal ? normal->createCopy() : nullptr);
        if (over)
            over->replaceColour(juce::Colours::white, juce::Colours::white.withAlpha(1.f));
        if (down)
            down->replaceColour(juce::Colours::white, juce::Colours::white.withAlpha(1.f));

        button->setImages(normal,
                          over.get(),
                          down.get(),
                          nullptr, nullptr, nullptr, nullptr, nullptr);

        // Keep ownership of state images so they persist
        overImage = std::move(over);
        downImage = std::move(down);
        repaint();
    }

    bool locked { true };
    std::unique_ptr<juce::DrawableButton> button;
    std::unique_ptr<juce::Drawable> lockedDrawable, unlockedDrawable;
    // store alternate state images we created
    std::unique_ptr<juce::Drawable> overImage, downImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpLock)
};
