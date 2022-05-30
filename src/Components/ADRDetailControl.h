#pragma once

#include "ADRControl.h"

class ADRDetailControl : public ADRControl
{
public:
    explicit ADRDetailControl(const Control &control) : ADRControl(control)
    {
    }

    virtual ~ADRDetailControl() = default;

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
    }

private:
};
