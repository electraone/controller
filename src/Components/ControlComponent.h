#pragma once

#include "Control.h"
#include "Component.h"

class ControlComponent
{
public:
    ControlComponent();
    virtual ~ControlComponent() = default;

    virtual void messageMatched(Value2 *value,
                                int16_t midiValue,
                                uint8_t handle = 1) = 0;

    static Component *createControlComponent(const Control &control);

private:
};
