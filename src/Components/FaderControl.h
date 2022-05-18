#pragma once

#include "Control.h"
#include "BarHorizontal.h"
#include "AssignableList.h"

class FaderControl : public BarHorizontal
{
public:
    explicit FaderControl(const Control &control)
    {
        setMinimum(control.values[0].getMin());
        setMaximum(control.values[0].getMax());
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
        list.assignListData(control.values[0].getOverlay());
    }

    virtual ~FaderControl() = default;

    void paint(Graphics &g) override
    {
        Rectangle sliderBounds = getBounds();

        paintFader(g,
                   sliderBounds,
                   colour,
                   value.getMin(),
                   value.getMax(),
                   value.get(),
                   list.getList());

        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
    }

    AssignableList list;

private:
    void paintFader(Graphics &g,
                    const Rectangle &bounds,
                    uint32_t colour,
                    int16_t min,
                    int16_t max,
                    int16_t val,
                    const ListData *items)
    {
        uint32_t colourTrack = Colours::darker(colour, 0.3f);

        uint16_t barHeight = 8;
        uint16_t padding = bounds.getHeight() - 35;

        uint16_t barX = map(0, min, max, 0, bounds.getWidth());
        uint16_t barWidth = map(val, min, max, 0, bounds.getWidth()) - barX;

        // Clear the component area
        g.fillAll(Colours::black);

        // Paint the track background
        g.setColour(colourTrack);
        g.fillRect(0, padding, bounds.getWidth(), barHeight);

        // Paint the active bar
        g.setColour(colour);
        g.fillRect(barX, padding, barWidth, barHeight);

        // Paint the outline
        g.drawRect(0, padding, bounds.getWidth(), barHeight);

        // Print the label text if exists
        if (items && !items->getByValue(val).isLabelEmpty()) {
            g.printText(0,
                        0,
                        items->getByValue(val).getLabel(),
                        TextStyle::mediumTransparent,
                        bounds.getWidth(),
                        TextAlign::center);
        } else {
            char stringValue[10];
            snprintf(stringValue, sizeof(stringValue), formatString, val);
            g.printText(0,
                        0,
                        stringValue,
                        TextStyle::mediumTransparent,
                        getWidth(),
                        TextAlign::center);
        }
    }
};
