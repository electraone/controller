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
        setColour(control.getColour565());
        updateValueFromParameterMap();
    }

    virtual ~PadControl() = default;

    virtual void onTouchDown(const TouchEvent &touchEvent) override
    {
        const ControlValue cv = control.getValue(0);
        uint16_t midiValue = cv.message.getOnValue();
        bool stateToPass = false;

        if (control.getMode() == Control::Mode::Toggle) {
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
        if (midiValue != MIDI_VALUE_DO_NOT_SEND) {
            cv.callFunction(stateToPass);
        }
    }

    virtual void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (control.getMode() == Control::Mode::Momentary) {
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

    virtual void onPotTouchDown(const PotEvent &potEvent) override
    {
        ControlComponent::onPotTouchDown(potEvent);
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
        if (control.getMode() == Control::Mode::Momentary) {
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
        ControlComponent::onPotTouchUp(potEvent);
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
        g.fillAll(getUseAltBackground() ? LookAndFeel::altBackgroundColour
                                        : LookAndFeel::backgroundColour);
        auto bounds = getBounds();
        auto isMomentary = (control.getMode() == Control::Mode::Momentary);
        bounds.setX(0);
        bounds.setY(3);
        bounds.setWidth(getWidth());
        bounds.setHeight(getHeight() - 6);
        LookAndFeel::paintPad(g, bounds, colour, isMomentary, getState());

        if (isMomentary) {
            g.setColour(Colours565::white);
        } else {
            g.setColour(getState() ? Colours565::white : Colours565::black);
        }
        g.print(0,
                getHeight() / 2 - 10,
                getName(),
                bounds.getWidth(),
                TextAlign::center);

        ControlComponent::paint(g);
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
