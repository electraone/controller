#pragma once

#include "ADSRControl.h"

class ADSRDetailControl : public ADSRControl
{
public:
    explicit ADSRDetailControl(const Control &control, UiDelegate *newDelegate)
        : ADSRControl(control, newDelegate)
    {
    }

    virtual ~ADSRDetailControl() = default;

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (!delegate->isDetailLocked()) {
            delegate->closeDetail();
        }
    }

    void onTouchLongHold(const TouchEvent &touchEvent) override
    {
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (!delegate->isDetailLocked()) {
            delegate->closeDetail();
        }
    }
};
