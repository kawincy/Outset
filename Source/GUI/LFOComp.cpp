/*
  ==============================================================================

    LFOComp.cpp
    Created: 2 Feb 2025 12:49:27pm
    Author:  josep

  ==============================================================================
*/

#include <JuceHeader.h>

#include "Colors.h"
#include "LFOComp.h"

LFOComp::LFOComp(juce::AudioProcessorValueTreeState& apvtsRef) : apvtsRef(apvtsRef)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    // setAlgIndexParameter(algo_ind); <-- setter
    algo_ind = apvtsRef.getRawParameterValue("ALG_INDEX")->load();

        
    /*addAndMakeVisible(test);
    addAndMakeVisible(test2);
    test2.set_carrier(false);
    test2.set_num(2);*/
    
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 6; c++) {
            addAndMakeVisible(grid[r][c]);
        }
    }
    rows = 4;
    cols = 6;
    box_width = 30;
    box_height = 20;

    spacing_x = 15;  // Space between columns
    spacing_y = 20;  // Space between rows
}

LFOComp::~LFOComp()
{
}

void LFOComp::paint (juce::Graphics& g)
{
    juce::Rectangle<int> bounds = getLocalBounds();

    g.fillAll(juce::Colour(colors().bg));

    g.setColour(juce::Colour(0x5B, 0x8F, 0x7E));

    auto select_width = bounds.getWidth() / 4;
    auto select_area = bounds.removeFromLeft(select_width);

    g.drawRect(bounds, 1);
    g.drawRect(select_area, 1);
    
}

void LFOComp::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    juce::Rectangle<int> bounds = getLocalBounds();
    auto select_width = bounds.getWidth() / 4;
    auto select_area = bounds.removeFromLeft(select_width);

    int rows = 4, cols = 6;
    int box_width = 30;
    int box_height = 20;

    int spacing_x = 15;  // Space between columns
    int spacing_y = 20;  // Space between rows

    int total_grid_width = cols * (box_width + spacing_x) - spacing_x;   // Account for spacing
    int total_grid_height = rows * (box_height + spacing_y) - spacing_y; // Account for spacing

    juce::Rectangle<int> algo_bounds = bounds.reduced(10);
    // Center the grid inside algo_bounds
    int startX = algo_bounds.getX() + (algo_bounds.getWidth() - total_grid_width) / 2;
    int startY = algo_bounds.getY() + (algo_bounds.getHeight() - total_grid_height) / 2;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            AlgoBoxComp& curr = grid[r][c];  // Use reference to avoid copy issues

            int x = startX + c * (box_width + spacing_x);
            int y = startY + r * (box_height + spacing_y);

            curr.setBounds(x, y, box_width, box_height);  // Set fixed bounds
        }
    }

}

void LFOComp::setAlgIndexParameter(int newValue)
{

    if (auto* param = apvtsRef.getParameter("ALG_INDEX"))
    {
        param->setValueNotifyingHost(newValue/31.0);  // we have to divide by 31 because setValueNotifyingHost takes a normalized value
    }
}


void LFOComp::a1_bounds() {
    juce::Rectangle<int> bounds = getLocalBounds();
    auto select_width = bounds.getWidth() / 4;
    juce::Rectangle<int> select_bounds = bounds.removeFromLeft(select_width);
    juce::Rectangle<int> algo_bounds = bounds.reduced(10);

    int total_grid_width = cols * (box_width + spacing_x) - spacing_x;   
    int total_grid_height = rows * (box_height + spacing_y) - spacing_y;

    int startX = algo_bounds.getX() + (algo_bounds.getWidth() - total_grid_width) / 2;
    int startY = algo_bounds.getY() + (algo_bounds.getHeight() - total_grid_height) / 2;
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            AlgoBoxComp& curr = grid[r][c];  // Use reference to avoid copy issues

            int x = startX + c * (box_width + spacing_x);
            int y = startY + r * (box_height + spacing_y);

            curr.setBounds(x, y, box_width, box_height);  // Set fixed bounds
        }
    }
}