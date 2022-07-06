#pragma once

#include "Detail.h"
#include "ControlComponent.h"
#include "KnobControl.h"

class DetailDx7Env : public Detail
{
public:
    DetailDx7Env(const Control &control, MainDelegate &newDelegate)
        : Detail(control, newDelegate),
          l1Knob(nullptr),
          l2Knob(nullptr),
          l3Knob(nullptr),
          l4Knob(nullptr),
          r1Knob(nullptr),
          r2Knob(nullptr),
          r3Knob(nullptr),
          r4Knob(nullptr)
    {
        l1Values.push_back(control.getValue(0));
        l1.setId(500);
        l1.setType(ControlType::knob);
        l1.setName("LEVEL 1");
        l1.setColour(control.getColour());
        l1.setValues(l1Values);
        l1.addToParameterMap(l1Values[0]);
        l1Knob = ControlComponent::createControlComponent(l1, delegate);
        l1Knob->updateValueFromParameterMap();
        l1Knob->assignPot(1, l1.values[0].getNumSteps());
        addAndMakeVisible(l1Knob);

        r1Values.push_back(control.getValue(1));
        r1.setId(501);
        r1.setType(ControlType::knob);
        r1.setName("RATE 1");
        r1.setColour(control.getColour());
        r1.setValues(r1Values);
        r1.addToParameterMap(r1Values[0]);
        r1Knob = ControlComponent::createControlComponent(r1, delegate);
        r1Knob->updateValueFromParameterMap();
        r1Knob->assignPot(7, r1.values[0].getNumSteps());
        addAndMakeVisible(r1Knob);

        l2Values.push_back(control.getValue(2));
        l2.setId(502);
        l2.setType(ControlType::knob);
        l2.setName("LEVEL 2");
        l2.setColour(control.getColour());
        l2.setValues(l2Values);
        l2.addToParameterMap(l2Values[0]);
        l2Knob = ControlComponent::createControlComponent(l2, delegate);
        l2Knob->updateValueFromParameterMap();
        l2Knob->assignPot(2, l2.values[0].getNumSteps());
        addAndMakeVisible(l2Knob);

        r2Values.push_back(control.getValue(3));
        r2.setId(503);
        r2.setType(ControlType::knob);
        r2.setName("RATE 2");
        r2.setColour(control.getColour());
        r2.setValues(r2Values);
        r2.addToParameterMap(r2Values[0]);
        r2Knob = ControlComponent::createControlComponent(r2, delegate);
        r2Knob->updateValueFromParameterMap();
        r2Knob->assignPot(8, r2.values[0].getNumSteps());
        addAndMakeVisible(r2Knob);

        l3Values.push_back(control.getValue(4));
        l3.setId(504);
        l3.setType(ControlType::knob);
        l3.setName("LEVEL 3");
        l3.setColour(control.getColour());
        l3.setValues(l3Values);
        l3.addToParameterMap(l3Values[0]);
        l3Knob = ControlComponent::createControlComponent(l3, delegate);
        l3Knob->updateValueFromParameterMap();
        l3Knob->assignPot(3, l3.values[0].getNumSteps());
        addAndMakeVisible(l3Knob);

        r3Values.push_back(control.getValue(5));
        r3.setId(505);
        r3.setType(ControlType::knob);
        r3.setName("RATE 3");
        r3.setColour(control.getColour());
        r3.setValues(r3Values);
        r3.addToParameterMap(r3Values[0]);
        r3Knob = ControlComponent::createControlComponent(r3, delegate);
        r3Knob->updateValueFromParameterMap();
        r3Knob->assignPot(9, r3.values[0].getNumSteps());
        addAndMakeVisible(r3Knob);

        l4Values.push_back(control.getValue(6));
        l4.setId(506);
        l4.setType(ControlType::knob);
        l4.setName("LEVEL 4");
        l4.setColour(control.getColour());
        l4.setValues(l4Values);
        l4.addToParameterMap(l4Values[0]);
        l4Knob = ControlComponent::createControlComponent(l4, delegate);
        l4Knob->updateValueFromParameterMap();
        l4Knob->assignPot(4, l4.values[0].getNumSteps());
        addAndMakeVisible(l4Knob);

        r4Values.push_back(control.getValue(7));
        r4.setId(507);
        r4.setType(ControlType::knob);
        r4.setName("RATE 4");
        r4.setColour(control.getColour());
        r4.setValues(r4Values);
        r4.addToParameterMap(r4Values[0]);
        r4Knob = ControlComponent::createControlComponent(r4, delegate);
        r4Knob->updateValueFromParameterMap();
        r4Knob->assignPot(10, r4.values[0].getNumSteps());
        addAndMakeVisible(r4Knob);

        Detail::setLocked();
    }

    virtual ~DetailDx7Env()
    {
        l1Knob->releasePot();
        l2Knob->releasePot();
        l3Knob->releasePot();
        l4Knob->releasePot();
        r1Knob->releasePot();
        r2Knob->releasePot();
        r3Knob->releasePot();
        r4Knob->releasePot();
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
        l1Knob->setBounds(63, getHeight() - 240, 75, 100);
        l2Knob->setBounds(228, getHeight() - 245, 75, 100);
        l3Knob->setBounds(394, getHeight() - 245, 75, 100);
        l4Knob->setBounds(560, getHeight() - 245, 75, 100);
        r1Knob->setBounds(63, getHeight() - 130, 75, 100);
        r2Knob->setBounds(228, getHeight() - 130, 75, 100);
        r3Knob->setBounds(394, getHeight() - 130, 75, 100);
        r4Knob->setBounds(560, getHeight() - 130, 75, 100);
    }

private:
    ControlComponent *l1Knob;
    Control l1;
    ControlComponent *l2Knob;
    Control l2;
    ControlComponent *l3Knob;
    Control l3;
    ControlComponent *l4Knob;
    Control l4;
    ControlComponent *r1Knob;
    Control r1;
    ControlComponent *r2Knob;
    Control r2;
    ControlComponent *r3Knob;
    Control r3;
    ControlComponent *r4Knob;
    Control r4;

    std::vector<ControlValue> l1Values;
    std::vector<ControlValue> l2Values;
    std::vector<ControlValue> l3Values;
    std::vector<ControlValue> l4Values;
    std::vector<ControlValue> r1Values;
    std::vector<ControlValue> r2Values;
    std::vector<ControlValue> r3Values;
    std::vector<ControlValue> r4Values;
};
