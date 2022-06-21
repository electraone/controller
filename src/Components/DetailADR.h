#pragma once

#include "Detail.h"
#include "ControlComponent.h"
#include "KnobControl.h"

class DetailADR : public Detail
{
public:
    DetailADR(const Control &control, UiDelegate *newDelegate)
        : Detail(control, newDelegate),
          attackKnob(nullptr),
          decayKnob(nullptr),
          releaseKnob(nullptr)
    {
        // \todo to be deduplicated
        attackValues.push_back(control.getValue(0));
        attack.setId(500);
        attack.setType(ControlType::knob);
        attack.setName("ATTACK");
        attack.setColour(control.getColour());
        attack.setValues(attackValues);
        attack.addToParameterMap(attackValues[0]);
        attackKnob = ControlComponent::createControlComponent(attack, delegate);
        attackKnob->updateValueFromParameterMap();
        attackKnob->assignPot(1, attack.values[0].getNumSteps());
        addAndMakeVisible(attackKnob);

        decayValues.push_back(control.getValue(1));
        decay.setId(501);
        decay.setType(ControlType::knob);
        decay.setName("DECAY");
        decay.setColour(control.getColour());
        decay.setValues(decayValues);
        decay.addToParameterMap(decayValues[0]);
        decayKnob = ControlComponent::createControlComponent(decay, delegate);
        decayKnob->updateValueFromParameterMap();
        decayKnob->assignPot(2, decay.values[0].getNumSteps());
        addAndMakeVisible(decayKnob);

        releaseValues.push_back(control.getValue(2));
        release.setId(502);
        release.setType(ControlType::knob);
        release.setName("RELEASE");
        release.setColour(control.getColour());
        release.setValues(releaseValues);
        release.addToParameterMap(releaseValues[0]);
        releaseKnob =
            ControlComponent::createControlComponent(release, delegate);
        releaseKnob->updateValueFromParameterMap();
        releaseKnob->assignPot(3, release.values[0].getNumSteps());
        addAndMakeVisible(releaseKnob);

        Detail::setLocked();
    }

    virtual ~DetailADR()
    {
        attackKnob->releasePot();
        decayKnob->releasePot();
        releaseKnob->releasePot();
    }

    void paint(Graphics &g)
    {
        g.setColour(Colours::black);
        g.fillRect(0, 0, getWidth() - 120, getHeight());
        g.setColour(Colours::darkgrey);
        g.drawRect(0, 0, getWidth() - 120, getHeight());
        g.dim(getWidth() - 120, 0, 120, getHeight(), Colours::black);
    }

    void resized(void)
    {
        component->setBounds(50, 50, 608, 150);
        lock->setBounds(getWidth() - 100, 25, 75, 75);
        dimmedSection->setBounds(getWidth() - 120, 100, 120, getHeight() - 100);
        attackKnob->setBounds(063, getHeight() - 130, 75, 100);
        decayKnob->setBounds(228, getHeight() - 130, 75, 100);
        releaseKnob->setBounds(394, getHeight() - 130, 75, 100);
    }

private:
    ControlComponent *attackKnob;
    Control attack;
    ControlComponent *decayKnob;
    Control decay;
    ControlComponent *releaseKnob;
    Control release;

    std::vector<ControlValue> attackValues;
    std::vector<ControlValue> decayValues;
    std::vector<ControlValue> releaseValues;
};
