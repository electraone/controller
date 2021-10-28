#pragma once

#include "Control.h"
#include "BarHorizontal.h"

class FaderControl : public BarHorizontal
{
public:
    explicit FaderControl(const Control &control)
    {
        setMinimum(control.values[0].getMin());
        setMaximum(control.values[0].getMax());
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~FaderControl() = default;

    void paint(Graphics &g) override
    {
        Rectangle sliderBounds = getBounds();
        sliderBounds.setHeight(23);
        LookAndFeel::paintBarHorizontal(g,
                                        sliderBounds,
                                        colour,
                                        value.getMin(),
                                        value.getMax(),
                                        value.get());
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
