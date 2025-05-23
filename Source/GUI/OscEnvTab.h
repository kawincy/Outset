/*
  ==============================================================================

    OscEnvTab.h
    Created: 4 Feb 2025 2:22:46pm
    Author:  ryanb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Colors.h"
#include "OscEnvParent.h"

// This component acts as the parent for the OscEnvParents. Switching tabs makes one of the 6 OscEnv Parents Visible.
class OscEnvTab : public juce::TabbedComponent
{
public:
    OscEnvTab(juce::AudioProcessorValueTreeState& apvtsRef)
        : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop)
    {
        setTabBarDepth(30);

        addTab("Osc 1", colors().main, new OscEnvParent(1, apvtsRef), true);
        addTab("Osc 2", colors().main, new OscEnvParent(2, apvtsRef), true);
        addTab("Osc 3", colors().main, new OscEnvParent(3, apvtsRef), true);
        addTab("Osc 4", colors().main, new OscEnvParent(4, apvtsRef), true);
        addTab("Osc 5", colors().main, new OscEnvParent(5, apvtsRef), true);
        addTab("Osc 6", colors().main, new OscEnvParent(6, apvtsRef), true);
    }

    ~OscEnvTab() override = default;
};
