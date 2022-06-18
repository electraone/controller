#pragma once

#include "Dx7EnvControl.h"

class Dx7EnvDetailControl : public Dx7EnvControl
{
public:
    explicit Dx7EnvDetailControl(const Control &control,
                                 UiDelegate *newDelegate)
        : Dx7EnvControl(control, newDelegate)
    {
    }

    virtual ~Dx7EnvDetailControl() = default;

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
