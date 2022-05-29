#pragma once

#include "Control.h"
#include "Component.h"
#include "ParameterMap.h"

class ControlComponent : virtual public Component
{
public:
    ControlComponent(const Control &controlToAssign);
    virtual ~ControlComponent() = default;

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 1) = 0;

    void onTouchLongHold(const TouchEvent &touchEvent) override;
    virtual void emitValueChange(int16_t newDisplayValue,
                                 const ControlValue &cv);

    static Component *createControlComponent(const Control &control,
                                             bool detail = false);

protected:
    const Control &control;
};
