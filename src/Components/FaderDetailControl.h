#pragma once

#include "FaderControl.h"

class FaderDetailControl : public FaderControl
{
public:
    explicit FaderDetailControl(const Control &control, UiDelegate *newDelegate)
        : FaderControl(control), delegate(newDelegate)
    {
    }

    virtual ~FaderDetailControl() = default;

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

private:
    UiDelegate *delegate;
};
