#pragma once

#include "Control.h"
#include "Pad.h"

class PadControl : public Pad
{
public:
    explicit PadControl(const Control &control)
    {
        setState(false);
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~PadControl() = default;

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

		g.setColour(Colours::white);
		auto bounds = getBounds();
		g.drawRect(0, 0, bounds.getWidth(), bounds.getHeight());
    }

private:
};
