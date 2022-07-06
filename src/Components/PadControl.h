#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "Pad.h"

class PadControl : public ControlComponent, public Pad
{
public:
    PadControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate)
    {
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
        updateValueFromParameterMap();
    }

    virtual ~PadControl() = default;

    virtual void onTouchDown(const TouchEvent &touchEvent) override
    {
        const ControlValue cv = control.getValue(0);
        uint16_t midiValue = cv.message.getOnValue();
        bool stateToPass = false;

        if (control.getMode() == ControlMode::toggle) {
            midiValue = (state == false) ? cv.message.getOnValue()
                                         : cv.message.getOffValue();
            setState(!state);
            stateToPass = state;
        } else {
            stateToPass = true;
        }

        parameterMap.setValue(cv.message.getDeviceId(),
                              cv.message.getType(),
                              cv.message.getParameterNumber(),
                              midiValue,
                              Origin::internal);
        // if this is used. The pad will emit the MIDI value as the value
        //cv.callFunction(midiValue);
        // For compatibility reasons it emits the state
        if (midiValue != MIDI_VALUE_DO_NOT_SEND) {
            cv.callFunction(stateToPass);
        }
    }

    virtual void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (control.getMode() == ControlMode::momentary) {
            const ControlValue cv = control.getValue(0);
            parameterMap.setValue(cv.message.getDeviceId(),
                                  cv.message.getType(),
                                  cv.message.getParameterNumber(),
                                  cv.message.getOffValue(),
                                  Origin::internal);

            // if this is used. The pad will emit the MIDI value as the value
            //cv.callFunction(cv.message.getOffValue());
            // For compatibility reasons it emits the state
            if (cv.message.getOffValue() != MIDI_VALUE_DO_NOT_SEND) {
                cv.callFunction(false);
            }
        }
    }

    virtual void onPotChange(const PotEvent &potEvent) override
    {
        int16_t delta = potEvent.getAcceleratedChange();
        int8_t step = (delta < 0) - (delta > 0);
        const ControlValue cv = control.getValue(0);

        if (step < 0 && state != true) {
            parameterMap.setValue(cv.message.getDeviceId(),
                                  cv.message.getType(),
                                  cv.message.getParameterNumber(),
                                  cv.message.getOnValue(),
                                  Origin::internal);
            if (cv.message.getOnValue() != MIDI_VALUE_DO_NOT_SEND) {
                cv.callFunction(true);
            }
        } else if (step > 0 && state != false) {
            parameterMap.setValue(cv.message.getDeviceId(),
                                  cv.message.getType(),
                                  cv.message.getParameterNumber(),
                                  cv.message.getOffValue(),
                                  Origin::internal);
            if (cv.message.getOffValue() != MIDI_VALUE_DO_NOT_SEND) {
                cv.callFunction(false);
            }
        }
    }

    virtual void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (control.getMode() == ControlMode::momentary) {
            const ControlValue cv = control.getValue(0);

            parameterMap.setValue(cv.message.getDeviceId(),
                                  cv.message.getType(),
                                  cv.message.getParameterNumber(),
                                  cv.message.getOffValue(),
                                  Origin::internal);
            if (cv.message.getOffValue() != MIDI_VALUE_DO_NOT_SEND) {
                cv.callFunction(false);
            }
        }
    }

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) override
    {
        if (value.message.getOnValue() == midiValue) {
            setState(true);
        } else {
            setState(false);
        }
    }

    void paint(Graphics &g) override
    {
        auto bounds = getBounds();
        bounds.setX(0);
        bounds.setY(3);
        bounds.setWidth(getWidth());
        bounds.setHeight(getHeight() - 6);
        LookAndFeel::paintPad(g, bounds, colour, getState());
        g.printText(
            2,
            getHeight() / 2 - 7,
            getName(),
            getState() ? TextStyle::mediumBlackOnWhite
                       : TextStyle::mediumWhiteOnBlack,
            bounds.getWidth(),
            TextAlign::center,
            control.getColour()); // needed to use the legacy text colours
    }

    void emitValueChange(int16_t newDisplayValue, const ControlValue &cv)
    {
        // left empty on pupose. The logic is handled in the Pad component
    }

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
        // left empty on pupose. It overrides the long hold of ControlComponent
        // which defaults to displaying the detail.
    }
};
