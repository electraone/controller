#pragma once

#include "Component.h"
#include "Control.h"

class ControlComponent
{
public:
    virtual ~ControlComponent() = default;

    static Component *createControlComponent(const Control &control);

private:
};
