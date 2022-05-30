#pragma once

#include "ADSRControl.h"

class ADSRDetailControl : public ADSRControl
{
public:
    explicit ADSRDetailControl(const Control &control) : ADSRControl(control)
    {
    }

    virtual ~ADSRDetailControl() = default;

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
    }

private:
};
