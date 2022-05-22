#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "ADSR.h"

class ADSRControl : public ControlComponent, public ADSR
{
public:
    explicit ADSRControl(const Control &control)
    {
        setMin(ADSR::attack, control.values[0].getMin());
        setMax(ADSR::attack, control.values[0].getMax());
        setValue(ADSR::attack, control.values[0].getDefault());

        setMin(ADSR::decay, control.values[1].getMin());
        setMax(ADSR::decay, control.values[1].getMax());
        setValue(ADSR::decay, control.values[1].getDefault());

        setMin(ADSR::sustain, control.values[2].getMin());
        setMax(ADSR::sustain, control.values[2].getMax());
        setValue(ADSR::sustain, control.values[2].getDefault());

        setMin(ADSR::release, control.values[3].getMin());
        setMax(ADSR::release, control.values[3].getMax());
        setValue(ADSR::release, control.values[3].getDefault());

        setActiveSegment(ADSR::sustain);

        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~ADSRControl() = default;

    void messageMatched(Value2 *value,
                        int16_t midiValue,
                        uint8_t handle = 1) override
    {
        int16_t newDisplayValue =
            translateMidiValueToValue(value->message.getSignMode(),
                                      value->message.getBitWidth(),
                                      midiValue,
                                      value->message.getMidiMin(),
                                      value->message.getMidiMax(),
                                      value->getMin(),
                                      value->getMax());

        if (1 <= handle && handle <= 4) {
            setValue(handle, newDisplayValue);
        }
    }

    void paint(Graphics &g) override
    {
        Rectangle envBounds = getBounds();
        envBounds.setHeight(envBounds.getHeight() / 2);
        computePoints(envBounds);
        LookAndFeel::paintEnvelope(g, envBounds, colour, baselineY, points);

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
