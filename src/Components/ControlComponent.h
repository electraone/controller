#pragma once

#include "Control.h"
#include "Component.h"
#include "ParameterMap.h"
#include "MainDelegate.h"

class ControlComponent : virtual public Component
{
public:
    ControlComponent(const Control &controlToAssign, MainDelegate &newDelegate);
    virtual ~ControlComponent() = default;

    virtual void onControlUpdated(void);
    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) = 0;
    virtual void updateValueFromParameterMap(void);
    virtual void onTouchLongHold(const TouchEvent &touchEvent) override;
    virtual void onTouchDoubleClick(const TouchEvent &touchEvent) override;
    virtual void emitValueChange(int16_t newDisplayValue,
                                 const ControlValue &cv);
    void setUseAltBackground(bool shouldUseAltBackground);
    bool getUseAltBackground(void) const;
    static ControlComponent *createControlComponent(const Control &control,
                                                    MainDelegate &newDelegate);
    static ControlComponent *
        createDetailControlComponent(const Control &control,
                                     MainDelegate &newDelegate);

protected:
    const Control &control;
    MainDelegate &delegate;
    bool useAltBackground;
};
