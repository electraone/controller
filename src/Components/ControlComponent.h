#pragma once

#include "Control.h"
#include "Component.h"
#include "ParameterMap.h"

class ControlComponent
{
public:
    ControlComponent(const Control &controlToAssign);
    virtual ~ControlComponent() = default;

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 1) = 0;

    static Component *createControlComponent(const Control &control);

protected:
    const Control &control;
};
