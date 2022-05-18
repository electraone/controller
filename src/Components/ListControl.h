#pragma once

#include "Control.h"
#include "List.h"

class ListControl : public List
{
public:
    explicit ListControl(const Control &control)
        : List(control.values[0].getOverlay())
    {
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~ListControl() = default;

    void paint(Graphics &g) override
    {
        logMessage("paint list");

        Rectangle sliderBounds = getBounds();
        sliderBounds.setHeight(sliderBounds.getHeight() / 2);
        LookAndFeel::paintList(g, sliderBounds, colour, getList(), index);
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
