#pragma once

#include "Detail.h"
#include "ControlComponent.h"
#include "KnobControl.h"

class DetailADSR : public Detail
{
public:
    DetailADSR(const Control &control, MainDelegate &newDelegate)
        : Detail(control, newDelegate),
          attackKnob(nullptr),
          decayKnob(nullptr),
          sustainKnob(nullptr),
          releaseKnob(nullptr)
    {
        // \todo to be deduplicated
        attackValues.push_back(control.getValue(0));
        attack.setId(500);
        attack.setType(ControlType::knob);
        attack.setName("ATTACK");
        attack.setColour(control.getColour());
        attack.setValues(attackValues);
        attackValues[0].setControl(&attack);
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
        decayValues[0].setControl(&decay);
        decay.addToParameterMap(decayValues[0]);
        decayKnob = ControlComponent::createControlComponent(decay, delegate);
        decayKnob->updateValueFromParameterMap();
        decayKnob->assignPot(2, decay.values[0].getNumSteps());
        addAndMakeVisible(decayKnob);

        sustainValues.push_back(control.getValue(2));
        sustain.setId(502);
        sustain.setType(ControlType::knob);
        sustain.setName("SUSTAIN");
        sustain.setColour(control.getColour());
        sustain.setValues(sustainValues);
        sustainValues[0].setControl(&sustain);
        sustain.addToParameterMap(sustainValues[0]);
        sustainKnob =
            ControlComponent::createControlComponent(sustain, delegate);
        sustainKnob->updateValueFromParameterMap();
        sustainKnob->assignPot(3, sustain.values[0].getNumSteps());
        addAndMakeVisible(sustainKnob);

        releaseValues.push_back(control.getValue(3));
        release.setId(503);
        release.setType(ControlType::knob);
        release.setName("RELEASE");
        release.setColour(control.getColour());
        release.setValues(releaseValues);
        releaseValues[0].setControl(&release);
        release.addToParameterMap(releaseValues[0]);
        releaseKnob =
            ControlComponent::createControlComponent(release, delegate);
        releaseKnob->updateValueFromParameterMap();
        releaseKnob->assignPot(4, release.values[0].getNumSteps());
        addAndMakeVisible(releaseKnob);
        parameterMap.print();
        Detail::setLocked();
    }

    virtual ~DetailADSR()
    {
        attack.removeFromParameterMap(attackValues[0]);
        decay.removeFromParameterMap(decayValues[0]);
        sustain.removeFromParameterMap(sustainValues[0]);
        release.removeFromParameterMap(releaseValues[0]);
        attackKnob->releasePot();
        decayKnob->releasePot();
        sustainKnob->releasePot();
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
        sustainKnob->setBounds(394, getHeight() - 130, 75, 100);
        releaseKnob->setBounds(560, getHeight() - 130, 80, 100);
    }

private:
    ControlComponent *attackKnob;
    Control attack;
    ControlComponent *decayKnob;
    Control decay;
    ControlComponent *sustainKnob;
    Control sustain;
    ControlComponent *releaseKnob;
    Control release;

    std::vector<ControlValue> attackValues;
    std::vector<ControlValue> decayValues;
    std::vector<ControlValue> sustainValues;
    std::vector<ControlValue> releaseValues;
};
