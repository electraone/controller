#pragma once

#include "Control.h"
#include "ADR.h"

class ADRControl : public ADR
{
public:
    explicit ADRControl(const Control &control)
    {
        setMin(ADR::attack, control.values[0].getMin());
        setMax(ADR::attack, control.values[0].getMax());
        setValue(ADR::attack, control.values[0].getDefault());

        setMin(ADR::decay, control.values[1].getMin());
        setMax(ADR::decay, control.values[1].getMax());
        setValue(ADR::decay, control.values[1].getDefault());

        setMin(ADR::release, control.values[2].getMin());
        setMax(ADR::release, control.values[2].getMax());
        setValue(ADR::release, control.values[2].getDefault());

        setActiveSegment(ADR::attack);

        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~ADRControl() = default;

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
