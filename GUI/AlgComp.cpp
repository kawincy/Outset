/*
  ==============================================================================

    AlgComp.cpp
    Created: 2 Feb 2025 12:49:27pm
    Author:  josep

  ==============================================================================
*/

#include <JuceHeader.h>

#include "Colors.h"
#include "AlgComp.h"

AlgComp::AlgComp(juce::AudioProcessorValueTreeState& apvtsRef) : apvtsRef(apvtsRef)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
//    algo_ind = apvtsRef.getRawParameterValue("ALG_INDEX")->load();// joe code

    juce::Path next_trianglePath, prev_trianglePath;
    next_trianglePath.addTriangle(0, 0, 30, 15, 0, 30);
    prev_trianglePath.addTriangle(30, 0, 0, 15, 30, 30);

    auto drawable = std::make_unique<juce::DrawablePath>();
    drawable->setPath(next_trianglePath);
    drawable->setFill(colors().white);

    next_b = std::make_unique<juce::DrawableButton>("next", juce::DrawableButton::ImageFitted);
    next_b->setImages(drawable.get());
    addAndMakeVisible(next_b.get());

    drawable->setPath(prev_trianglePath);

    prev_b = std::make_unique<juce::DrawableButton>("prev", juce::DrawableButton::ImageFitted);
    prev_b->setImages(drawable.get());
    addAndMakeVisible(prev_b.get());


    // Attach button click logic
    next_b->onClick = [this, &apvtsRef = apvtsRef]() {

        int algo_ind = (apvtsRef.getRawParameterValue("ALG_INDEX")->load());

        setAlgIndexParameter((algo_ind + 1) % 32);

        repaint();
    };

    prev_b->onClick = [this, &apvtsRef = apvtsRef]() {
        int algo_ind = (apvtsRef.getRawParameterValue("ALG_INDEX")->load());
        if (--algo_ind == -1) algo_ind = 31;
        setAlgIndexParameter(algo_ind);
        repaint();
    };

    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_1_png, BinaryData::algorithm_1_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_2_png, BinaryData::algorithm_2_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_3_png, BinaryData::algorithm_3_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_4_png, BinaryData::algorithm_4_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_5_png, BinaryData::algorithm_5_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_6_png, BinaryData::algorithm_6_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_7_png, BinaryData::algorithm_7_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_8_png, BinaryData::algorithm_8_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_9_png, BinaryData::algorithm_9_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_10_png, BinaryData::algorithm_10_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_11_png, BinaryData::algorithm_11_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_12_png, BinaryData::algorithm_12_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_13_png, BinaryData::algorithm_13_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_14_png, BinaryData::algorithm_14_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_15_png, BinaryData::algorithm_15_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_16_png, BinaryData::algorithm_16_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_17_png, BinaryData::algorithm_17_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_18_png, BinaryData::algorithm_18_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_19_png, BinaryData::algorithm_19_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_20_png, BinaryData::algorithm_20_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_21_png, BinaryData::algorithm_21_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_22_png, BinaryData::algorithm_22_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_23_png, BinaryData::algorithm_23_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_24_png, BinaryData::algorithm_24_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_25_png, BinaryData::algorithm_25_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_26_png, BinaryData::algorithm_26_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_27_png, BinaryData::algorithm_27_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_28_png, BinaryData::algorithm_28_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_29_png, BinaryData::algorithm_29_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_30_png, BinaryData::algorithm_30_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_31_png, BinaryData::algorithm_31_pngSize));
    images.add(juce::ImageFileFormat::loadFrom(BinaryData::algorithm_32_png, BinaryData::algorithm_32_pngSize));


    image = images[(apvtsRef.getRawParameterValue("ALG_INDEX")->load())];
   
}

AlgComp::~AlgComp()
{
}

void AlgComp::paint (juce::Graphics& g)
{
    juce::Rectangle<int> bounds = getLocalBounds();
    g.fillAll(colors().bg);

    g.setColour(colors().accent);
    g.drawRect(bounds, 1);

    g.setColour(colors().black);
    g.setFont(juce::FontOptions(14.0f));

    int x = (bounds.getWidth() / 2) - bounds.getHeight() / 3;
    int y = bounds.getHeight() - bounds.getHeight() / 8;
    juce::Rectangle<int> centeredRect(bounds.getWidth()/8, 0, bounds.getWidth() * 3/ 4, bounds.getHeight() * 7 / 8);

    g.setColour(colors().gridlines);
    g.fillRect(centeredRect);

    g.setColour(colors().main);
    g.drawRect(centeredRect, 2);

    image = images[static_cast<int>(apvtsRef.getRawParameterValue("ALG_INDEX")->load())];
    if (image.isValid())
    {
        // Get component bounds and draw image centered
        g.drawImageWithin(image, centeredRect.getX(), centeredRect.getY(), centeredRect.getWidth(), centeredRect.getHeight(),
            juce::RectanglePlacement::yBottom);

        g.drawText("Algorithm: " + std::to_string(static_cast<int>(apvtsRef.getRawParameterValue("ALG_INDEX")->load()) + 1), x, y + 2, bounds.getHeight() / 3 * 2, 20, juce::Justification::centred, true);
    }
}

void AlgComp::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    juce::Rectangle<int> bounds = getLocalBounds();

    next_b->setBounds(bounds.getWidth() * 2 / 3, bounds.getHeight() * 7 / 8 + 2, 18, 18);
    prev_b->setBounds(bounds.getWidth() / 3 - 20, bounds.getHeight() * 7 / 8 + 2, 18, 18);
}

void AlgComp::setAlgIndexParameter(int newValue)
{

    if (auto* param = apvtsRef.getParameter("ALG_INDEX"))
    {
        param->setValueNotifyingHost(newValue/31.0);  // we have to divide by 31 because setValueNotifyingHost takes a normalized value
    }
}
