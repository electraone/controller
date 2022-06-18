#pragma once

#include "ADRControl.h"

class ADRDetailControl : public ADRControl
{
public:
    explicit ADRDetailControl(const Control &control, UiDelegate *newDelegate)
        : ADRControl(control, newDelegate)
    {
    }

    virtual ~ADRDetailControl() = default;

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
