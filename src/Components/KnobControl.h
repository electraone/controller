#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "Knob.h"

class KnobControl : public ControlComponent, public Knob
{
public:
    explicit KnobControl(const Control &control) : ControlComponent(control)
    {
        setMinimum(control.values[0].getMin());
        setMaximum(control.values[0].getMax());
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~KnobControl() = default;

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = value.getMax();
        int16_t min = value.getMin();

        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        emitValueChange(newDisplayValue, control.getValue(0));
    }

    void onPotChange(const PotEvent &potEvent) override
    {
        if (potEvent.getRelativeChange()) {
            int16_t delta = potEvent.getAcceleratedChange();
            int16_t newDisplayValue = getValue() + delta;

            emitValueChange(newDisplayValue, control.getValue(0));
        }
    }

    void onMidiValueChange(const ControlValue &value,
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
