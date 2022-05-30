#pragma once

#include "Dx7EnvControl.h"

class Dx7EnvDetailControl : public Dx7EnvControl
{
public:
    explicit Dx7EnvDetailControl(const Control &control)
        : Dx7EnvControl(control)
    {
    }

    virtual ~Dx7EnvDetailControl() = default;

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
    }

private:
};
