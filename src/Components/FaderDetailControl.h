#pragma once

#include "FaderControl.h"

class FaderDetailControl : public FaderControl
{
public:
    FaderDetailControl(const Control &control, UiDelegate *newDelegate)
        : FaderControl(control, newDelegate)
    {
        assignPot(control.inputs[0].getPotId(),
                  control.values[0].getNumSteps());
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~FaderDetailControl() = default;

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = value.getMax();
        int16_t min = value.getMin();
        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        emitValueChange(newDisplayValue, control.getValue(0));
    }

    void paint(Graphics &g)
    {
        g.fillAll(Colours::black);
        BarHorizontal::paint(g);
    }

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (!delegate->isDetailLocked()) {
            delegate->closeDetail();
        }
    }

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (!delegate->isDetailLocked()) {
            delegate->closeDetail();
        }
    }
};
