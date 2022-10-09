#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "Knob.h"

class KnobControl : public ControlComponent, public Knob
{
public:
    explicit KnobControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate)
    {
        setMinimum(control.values[0].getMin());
        setMaximum(control.values[0].getMax());
        setColour(control.getColour565());
    }

    virtual ~KnobControl() = default;

    virtual void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = value.getMax();
        int16_t min = value.getMin();

        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        emitValueChange(newDisplayValue, control.getValue(0));
    }

    virtual void onPotTouchDown(const PotEvent &potEvent) override
    {
        ControlComponent::onPotTouchDown(potEvent);
    }

    virtual void onPotChange(const PotEvent &potEvent) override
    {
        if (int16_t delta = potEvent.getAcceleratedChange()) {
            int16_t newDisplayValue = getValue() + delta;
            emitValueChange(newDisplayValue, control.getValue(0));
        }
    }

    virtual void onPotTouchUp(const PotEvent &potEvent) override
    {
        ControlComponent::onPotTouchUp(potEvent);
    }

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) override
    {
        int16_t newDisplayValue =
            translateMidiValueToValue(value.message.getSignMode(),
                                      value.message.getBitWidth(),
                                      midiValue,
                                      value.message.getMidiMin(),
                                      value.message.getMidiMax(),
                                      value.getMin(),
                                      value.getMax());
        setValue(newDisplayValue);
    }

    void paint(Graphics &g) override
    {
        Rectangle knobBounds = getBounds();

        Knob::paint(g);

        char stringValue[10];
        if (!control.getValue(0).getFormatter().empty()) {
            control.getValue(0).callFormatter(
                getValue(), stringValue, sizeof(stringValue));
        } else {
            snprintf(
                stringValue, sizeof(stringValue), formatString, getValue());
        }
        g.printText(0,
                    getWidth() / 2 - 7,
                    stringValue,
                    TextStyle::mediumTransparent,
                    getWidth(),
                    TextAlign::center);

        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
    }

private:
};
