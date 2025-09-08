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
#include "OpLock.h"

// This component acts as the parent for the OscEnvParents. Switching tabs makes one of the 6 OscEnv Parents Visible.
class OscEnvTab : public juce::TabbedComponent
{
public:
    OscEnvTab(juce::AudioProcessorValueTreeState& apvtsRef)
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop), apvtsRef(apvtsRef)
    {
        setTabBarDepth(30);

        addTab("Osc 1", colors().main, new OscEnvParent(1, apvtsRef), true);
        addTab("Osc 2", colors().main, new OscEnvParent(2, apvtsRef), true);
        addTab("Osc 3", colors().main, new OscEnvParent(3, apvtsRef), true);
        addTab("Osc 4", colors().main, new OscEnvParent(4, apvtsRef), true);
        addTab("Osc 5", colors().main, new OscEnvParent(5, apvtsRef), true);
        addTab("Osc 6", colors().main, new OscEnvParent(6, apvtsRef), true);

        addAndMakeVisible(opLock);

        // Set env change callbacks for each tab's EnvComp
        for (int tabIndex = 0; tabIndex < getNumTabs(); ++tabIndex)
        {
            if (auto* parent = dynamic_cast<OscEnvParent*>(getTabContentComponent(tabIndex)))
            {
                parent->getEnvComp().onEnvChange = [this, tabIndex](float a, float d, float s, float r)
                {
                    if (!opLock.isLocked() || isPropagating) return;
                    propagateEnvelopeFrom(tabIndex, a, d, s, r);
                };
            }
        }

        opLock.onToggle = [this](bool locked)
        {
            if (!locked) return;
            auto idx = getCurrentTabIndex();
            if (auto* parent = dynamic_cast<OscEnvParent*>(getTabContentComponent(idx)))
            {
                auto& env = parent->getEnvComp();
                propagateEnvelopeFrom(idx, env.getAttack(), env.getDecay(), env.getSustain(), env.getRelease());
            }
        };
    }

    virtual void currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName) override
    {
		setTabBackgroundColour(currentTabIndex, colors().main);
        setTabBackgroundColour(newCurrentTabIndex, colors().accent);
        currentTabIndex = newCurrentTabIndex;
    }
    ~OscEnvTab() override = default;
    void resized() override
    {
        TabbedComponent::resized();
        // Place lock in top-right inside the content area (not overlapping tab buttons)
        auto area = getLocalBounds();
        auto tabBarHeight = getTabbedButtonBar().getHeight();
        auto contentArea = area.withTrimmedTop(tabBarHeight);
        const int size = 42;
        opLock.setBounds(area.removeFromRight(size + 6).removeFromTop(size + 6).withSizeKeepingCentre(size, size));
    }
private:
    void setParamValue(const juce::String& prefix, int oneBasedIndex, float value)
    {
        auto* p = apvtsRef.getParameter(prefix + juce::String(oneBasedIndex));
        if (!p) return;
        auto norm = p->getNormalisableRange().convertTo0to1(value);
        p->beginChangeGesture();
        p->setValueNotifyingHost(norm);
        p->endChangeGesture();
    }

	void propagateEnvelopeFrom(int sourceTab, float a, float d, float s, float r)
    {
        const int srcOneBased = sourceTab + 1;
        isPropagating = true;
        for (int tabIndex = 0; tabIndex < getNumTabs(); ++tabIndex)
        {
            if (tabIndex == sourceTab) continue;
            int oneBased = tabIndex + 1;
            setParamValue("ATTACK_",  oneBased, a);
            setParamValue("DECAY_",   oneBased, d);
            setParamValue("SUSTAIN_", oneBased, s);
            setParamValue("RELEASE_", oneBased, r);
        }
        isPropagating = false;
    }
	int currentTabIndex = 0;
    OpLock opLock;
    juce::AudioProcessorValueTreeState& apvtsRef;
    bool isPropagating { false };
};
