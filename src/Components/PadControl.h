#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "Pad.h"

class PadControl : public ControlComponent, public Pad
{
public:
    explicit PadControl(const Control &control) : ControlComponent(control)
    {
        setState(false);
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~PadControl() = default;

    void onTouchDown(const TouchEvent &touchEvent) override
    {
        uint16_t midiValue = control.values[0].message.getOnValue();

        if (control.getMode() == ControlMode::toggle) {
            midiValue = (state == false)
                            ? control.values[0].message.getOnValue()
                            : control.values[0].message.getOffValue();
        }

        parameterMap.setValue(control.values[0].message.getDeviceId(),
                              control.values[0].message.getType(),
                              control.values[0].message.getParameterNumber(),
                              midiValue,
                              Origin::internal);
    }

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (control.getMode() == ControlMode::momentary) {
            parameterMap.setValue(
                control.values[0].message.getDeviceId(),
                control.values[0].message.getType(),
                control.values[0].message.getParameterNumber(),
                control.values[0].message.getOffValue(),
                Origin::internal);
        }
    }

    void onPotChange(const PotEvent &potEvent) override
    {
        int16_t delta = potEvent.getAcceleratedChange();
        int8_t step = (delta < 0) - (delta > 0);
        const ControlValue cv = control.getValue(0);

        if (step < 0 && state != true) {
            parameterMap.setValue(
                control.values[0].message.getDeviceId(),
                control.values[0].message.getType(),
                control.values[0].message.getParameterNumber(),
                control.values[0].message.getOnValue(),
                Origin::internal);
        } else if (step > 0 && state != false) {
            parameterMap.setValue(
                control.values[0].message.getDeviceId(),
                control.values[0].message.getType(),
                control.values[0].message.getParameterNumber(),
                control.values[0].message.getOffValue(),
                Origin::internal);
        }
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (control.getMode() == ControlMode::momentary) {
            const ControlValue cv = control.getValue(0);

            parameterMap.setValue(cv.message.getDeviceId(),
                                  cv.message.getType(),
                                  cv.message.getParameterNumber(),
                                  cv.message.getOffValue(),
                                  Origin::internal);
        }
    }

    void onMidiValueChange(const ControlValue &value,
                           int16_t midiValue,
                           uint8_t handle = 1) override
    {
        if (value.message.getOnValue() == midiValue) {
            setState(true);
        } else {
            setState(false);
        }
    }

    void paint(Graphics &g) override
    {
        LookAndFeel::paintPad(g, getBounds(), colour, getState());

        g.printText(0,
                    getHeight() / 2 - 10,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
    }

    void emitValueChange(int16_t newDisplayValue, const ControlValue &cv)
    {
        // left empty on pupose. The logic is handled in the Pad component
    }

private:
};
