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


// boxIndex[algorithm number][box numbers + feedback boxes][indices]
// boxIndex[#][6 = feedback boxes][output, input]
constexpr int boxInds[32][7][2] = {
//      1      2      3      4      5      6      fb
    { {3,2}, {2,2}, {3,3}, {2,3}, {1,3}, {0,3}, {6,6} }, // alg 1
    { {3,2}, {2,2}, {3,3}, {2,3}, {1,3}, {0,3}, {2,2} },
    { {3,2}, {2,2}, {1,2}, {3,3}, {2,3}, {1,3}, {6,6} },
    { {3,2}, {2,2}, {1,2}, {3,3}, {2,3}, {1,3}, {4,6} },
    { {3,2}, {2,2}, {3,3}, {2,3}, {3,4}, {2,4}, {6,6} }, // alg 5
    { {3,2}, {2,2}, {3,3}, {2,3}, {3,4}, {2,4}, {5,6} },
    { {3,2}, {2,2}, {3,3}, {2,3}, {2,4}, {1,4}, {6,6} },
    { {3,2}, {2,2}, {3,3}, {2,3}, {2,4}, {1,4}, {4,4} },
    { {3,2}, {2,2}, {3,3}, {2,3}, {2,4}, {1,4}, {2,2} },
    { {3,3}, {2,3}, {1,3}, {3,2}, {2,1}, {2,2}, {3,3} }, // alg 10
    { {3,3}, {2,3}, {1,3}, {3,2}, {2,1}, {2,2}, {6,6} },
    { {3,3}, {2,3}, {3,1}, {2,0}, {2,1}, {2,2}, {2,2} },
    { {3,3}, {2,3}, {3,1}, {2,0}, {2,1}, {2,2}, {6,6} },
    { {3,2}, {2,2}, {3,3}, {2,3}, {1,3}, {1,4}, {6,6} },
    { {3,2}, {2,2}, {3,3}, {2,3}, {1,3}, {1,4}, {2,2} }, // alg 15
    { {3,3}, {2,2}, {2,3}, {1,3}, {2,4}, {1,4}, {6,6} },
    { {3,3}, {2,2}, {2,3}, {1,3}, {2,4}, {1,4}, {2,2} },
    { {3,3}, {2,2}, {2,3}, {2,4}, {1,4}, {0,4}, {3,3} },
    { {3,1}, {2,1}, {1,1}, {3,2}, {3,3}, {2,2}, {6,6} },
    { {3,1}, {3,2}, {2,1}, {3,4}, {2,3}, {2,4}, {3,3} }, // alg 20
    { {3,1}, {3,2}, {2,1}, {3,3}, {3,4}, {2,3}, {3,3} },
    { {3,1}, {2,1}, {3,2}, {3,3}, {3,4}, {2,3}, {6,6} },
    { {3,1}, {2,2}, {3,2}, {3,3}, {3,4}, {2,3}, {6,6} },
    { {3,0}, {3,1}, {3,2}, {3,3}, {3,4}, {2,3}, {6,6} },
    { {3,0}, {3,1}, {3,2}, {3,3}, {3,4}, {2,3}, {6,6} }, // alg 25
    { {3,1}, {3,2}, {2,2}, {3,4}, {2,3}, {2,4}, {6,6} },
    { {3,1}, {3,2}, {2,2}, {3,4}, {2,3}, {2,4}, {3,3} },
    { {3,2}, {2,2}, {3,3}, {2,3}, {1,3}, {3,4}, {5,5} },
    { {3,1}, {3,2}, {3,3}, {2,3}, {3,4}, {2,4}, {6,6} },
    { {3,1}, {3,2}, {3,3}, {2,3}, {1,3}, {3,4}, {5,5} }, // alg 30
    { {3,0}, {3,1}, {3,2}, {3,3}, {3,4}, {2,4}, {6,6} },
    { {3,0}, {3,1}, {3,2}, {3,3}, {3,4}, {3,5}, {6,6} },
};

LFOComp::LFOComp(juce::AudioProcessorValueTreeState& apvtsRef) : apvtsRef(apvtsRef)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    setAlgIndexParameter(0);
    algo_ind = 0;
    
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
        undraw_boxes();
        algo_ind = apvtsRef.getRawParameterValue("ALG_INDEX")->load();
        algo_ind = (algo_ind + 1) % 32;
        setAlgIndexParameter(algo_ind);
        repaint();
    };

    prev_b->onClick = [this, &apvtsRef = apvtsRef]() {
        undraw_boxes();
        algo_ind = apvtsRef.getRawParameterValue("ALG_INDEX")->load();
        if (--algo_ind == -1) algo_ind = 31;
        setAlgIndexParameter(algo_ind);
        repaint();
    };

        
    /*addAndMakeVisible(test);
    addAndMakeVisible(test2);
    test2.set_carrier(false);
    test2.set_num(2);*/
    
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 6; c++) {
            addAndMakeVisible(grid[r][c]);
            if (r == 3) {
                AlgoBoxComp& curr = grid[r][c];
                curr.set_carrier(true);
            }
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

    g.setColour(juce::Colour(colors().accent));

    auto select_width = bounds.getWidth() / 4;
    auto select_area = bounds.removeFromLeft(select_width);

    g.drawRect(bounds, 1);
    g.drawRect(select_area, 1);
    
    draw_boxes();
}

void LFOComp::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    next_b->setBounds(bounds.getWidth() * 2 / 3, bounds.getHeight() * 7 / 8 + 2, 18, 18);
    prev_b->setBounds(bounds.getWidth() / 3 - 20, bounds.getHeight() * 7 / 8 + 2, 18, 18);
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
            AlgoBoxComp& curr = grid[r][c];

            int x = startX + c * (box_width + spacing_x);
            int y = startY + r * (box_height + spacing_y);

            curr.setBounds(x, y, box_width, box_height);  // Set fixed bounds
        }
    }
}

void LFOComp::draw_boxes() {
    for (int i = 0; i < 6; i++)
    {
        int box_row = boxInds[algo_ind][i][0];
        int box_col = boxInds[algo_ind][i][1];
        AlgoBoxComp& curr = grid[box_row][box_col];

        curr.set_num(i+1);
        curr.set_existance(true);
    }
}

void LFOComp::undraw_boxes() {
    for (int i = 0; i < 6; i++)
    {
        int box_row = boxInds[algo_ind][i][0];
        int box_col = boxInds[algo_ind][i][1];
        AlgoBoxComp& curr = grid[box_row][box_col];
        curr.set_existance(false);
    }
}

void LFOComp::setAlgIndexParameter(int newValue)
{

    if (auto* param = apvtsRef.getParameter("ALG_INDEX"))
    {
        param->setValueNotifyingHost(newValue/31.0);  // we have to divide by 31 because setValueNotifyingHost takes a normalized value
    }
}

