#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "Pad.h"

class PadControl : public ControlComponent, public Pad
{
public:
    explicit PadControl(const Control &control)
    {
        setState(false);
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~PadControl() = default;

    void messageMatched(Value2 *value,
                        int16_t midiValue,
                        uint8_t handle = 1) override
    {
        if (value->message.getOnValue() == midiValue) {
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

private:
};
