#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "List.h"

class ListControl : public ControlComponent, public List
{
public:
    explicit ListControl(const Control &control)
        : List(control.values[0].getOverlay())
    {
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~ListControl() = default;

    void messageMatched(Value2 *value,
                        int16_t midiValue,
                        uint8_t handle = 1) override
    {
        auto list = getList();

        if (list) {
            int16_t index = list->getIndex(midiValue);

            if (index >= 0) {
                setIndex(index);
            }
        }
    }

    void paint(Graphics &g) override
    {
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
