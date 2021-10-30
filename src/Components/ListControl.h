#pragma once

#include "Control.h"
#include "List.h"

class ListControl : public List
{
public:
    explicit ListControl(const Control &control)
    {
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~ListControl() = default;

    void paint(Graphics &g) override
    {
        Rectangle sliderBounds = getBounds();
        sliderBounds.setHeight(sliderBounds.getHeight() / 2);
        LookAndFeel::paintList(g, sliderBounds, colour, items, index);
        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
    }

private:
    static Overlay empty;
};
