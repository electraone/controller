#pragma once

#include "Control.h"
#include "Env5Seg.h"

class Dx7EnvControl : public Env5Seg
{
public:
    explicit Dx7EnvControl(const Control &control)
    {
        values[Env5Seg::level1].setMin(control.values[0].getMin());
        values[Env5Seg::level1].setMax(control.values[0].getMax());
        values[Env5Seg::level1].set(control.values[0].getDefault());

        values[Env5Seg::rate1].setMin(control.values[1].getMin());
        values[Env5Seg::rate1].setMax(control.values[1].getMax());
        values[Env5Seg::rate1].set(control.values[1].getDefault());

        values[Env5Seg::level2].setMin(control.values[2].getMin());
        values[Env5Seg::level2].setMax(control.values[2].getMax());
        values[Env5Seg::level2].set(control.values[2].getDefault());

        values[Env5Seg::rate2].setMin(control.values[3].getMin());
        values[Env5Seg::rate2].setMax(control.values[3].getMax());
        values[Env5Seg::rate2].set(control.values[3].getDefault());

        values[Env5Seg::level3].setMin(control.values[4].getMin());
        values[Env5Seg::level3].setMax(control.values[4].getMax());
        values[Env5Seg::level3].set(control.values[4].getDefault());

        values[Env5Seg::rate3].setMin(control.values[5].getMin());
        values[Env5Seg::rate3].setMax(control.values[5].getMax());
        values[Env5Seg::rate3].set(control.values[5].getDefault());

        values[Env5Seg::level4].setMin(control.values[6].getMin());
        values[Env5Seg::level4].setMax(control.values[6].getMax());
        values[Env5Seg::level4].set(control.values[6].getDefault());

        values[Env5Seg::rate4].setMin(control.values[7].getMin());
        values[Env5Seg::rate4].setMax(control.values[7].getMax());
        values[Env5Seg::rate4].set(control.values[7].getDefault());

        setActiveSegment(Env5Seg::level4);

        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~Dx7EnvControl() = default;

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
