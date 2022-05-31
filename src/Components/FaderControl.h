#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "BarHorizontal.h"
#include "AssignableList.h"

class FaderControl : public ControlComponent, public BarHorizontal
{
public:
    explicit FaderControl(const Control &control) : ControlComponent(control)
    {
        setMinimum(control.values[0].getMin());
        setMaximum(control.values[0].getMax());
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
        list.assignListData(control.values[0].getOverlay());
    }

    virtual ~FaderControl() = default;

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = value.getMax();
        int16_t min = value.getMin();

        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        emitValueChange(newDisplayValue, control.getValue(0));
    }

    void onPotChange(const PotEvent &potEvent) override
    {
        if (potEvent.getRelativeChange()) {
            int16_t delta = potEvent.getAcceleratedChange();
            int16_t newDisplayValue = getValue() + delta;

            emitValueChange(newDisplayValue, control.getValue(0));
        }
    }

    void onMidiValueChange(const ControlValue &value,
                           int16_t midiValue,
                           uint8_t handle = 1) override
    {
        int16_t newDisplayValue =
            translateMidiValueToValue(value.message.getSignMode(),
                                      value.message.getBitWidth(),
                                      midiValue,
                                      value.message.getMidiMin(),
                                      value.message.getMidiMax(),
                                      value.getMin(),
                                      value.getMax());
        setValue(newDisplayValue);
    }

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
        g.setColour(colourTrack);
        g.fillRect(barX, padding - 16, barWidth, 16);

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
            if (!control.getValue(0).getFormatter().empty()) {
                control.getValue(0).callFormatter(
                    val, stringValue, sizeof(stringValue));
            } else {
                snprintf(stringValue, sizeof(stringValue), formatString, val);
            }
            g.printText(0,
                        0,
                        stringValue,
                        TextStyle::mediumTransparent,
                        getWidth(),
                        TextAlign::center);
        }
    }
};
