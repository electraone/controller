#pragma once

#include "Control.h"
#include "Component.h"
#include "ParameterMap.h"
#include "UiDelegate.h"

class ControlComponent : virtual public Component
{
public:
    ControlComponent(const Control &controlToAssign);
    virtual ~ControlComponent() = default;

    virtual void onControlUpdated(void);
    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) = 0;
    virtual void updateValueFromParameterMap(void);
    virtual void onTouchLongHold(const TouchEvent &touchEvent) override;
    virtual void emitValueChange(int16_t newDisplayValue,
                                 const ControlValue &cv);

    static Component *createControlComponent(const Control &control);
    static Component *createDetailControlComponent(const Control &control,
                                                   UiDelegate *newDelegate);

protected:
    const Control &control;
};
