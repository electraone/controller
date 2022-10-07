#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "BarHorizontal.h"
#include "AssignableList.h"

class FaderControl : public ControlComponent, public BarHorizontal
{
public:
    FaderControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate),
          previousScreenX(0),
          thresholdCrossed(false)
    {
        const auto &controlValue = control.getValue(0);

        setMinimum(controlValue.getMin());
        setMaximum(controlValue.getMax());
        list.assignListData(controlValue.getOverlay());
        updateValueFromParameterMap();
    }

    virtual ~FaderControl() = default;

    virtual void onTouchDown(const TouchEvent &touchEvent) override
    {
        previousScreenX = touchEvent.getScreenX();
        thresholdCrossed = false;
    }

    virtual void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = value.getMax();
        int16_t min = value.getMin();
        float step = getWidth() / (float)(max - min);
        int16_t delta = touchEvent.getScreenX() - previousScreenX;

        if (!thresholdCrossed) {
            if (delta < 10) {
                return;
            } else {
                thresholdCrossed = true;
                delta = 0;
            }
        }
        previousScreenX = touchEvent.getScreenX();

        int16_t newDisplayValue =
            constrain(ceil(getValue() + ((float)delta / step)), min, max);
        emitValueChange(newDisplayValue, control.getValue(0));
    }

    virtual void onPotTouchDown(const PotEvent &potEvent) override
    {
        ControlComponent::onPotTouchDown(potEvent);
    }

    virtual void onPotChange(const PotEvent &potEvent) override
    {
        if (int16_t delta = potEvent.getAcceleratedChange()) {
            int16_t newDisplayValue = getValue() + delta;
            emitValueChange(newDisplayValue, control.getValue(0));
        }
    }

    virtual void onPotTouchUp(const PotEvent &potEvent) override
    {
        ControlComponent::onPotTouchUp(potEvent);
    }

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) override
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
        colour = control.getColour();
        Rectangle sliderBounds = getBounds();

        paintFader(g,
                   sliderBounds,
                   colour,
                   value.getMin(),
                   value.getMax(),
                   value.get(),
                   list.getList(),
                   control.getMode(),
                   control.getVariant());

        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
        ControlComponent::paint(g);
    }

    AssignableList list;

private:
    void paintFader(Graphics &g,
                    const Rectangle &bounds,
                    uint32_t colour,
                    int16_t min,
                    int16_t max,
                    int16_t val,
                    const ListData *items,
                    Control::Mode mode,
                    Control::Variant variant)
    {
        uint32_t colourTrack = Colours::darker(colour, 0.3f);
        uint32_t backgroundColour = getUseAltBackground()
                                        ? LookAndFeel::altBackgroundColour
                                        : LookAndFeel::backgroundColour;

        if (items) {
            variant = Control::Variant::Thin;
        }

        uint16_t barHeight = 0;
        uint16_t padding = 0;

        if (variant == Control::Variant::Thin) {
            barHeight = 9;
            padding = bounds.getHeight() - 36;
        } else {
            barHeight = 24;
            padding = bounds.getHeight() - 51;
        }

        uint16_t barX = 0;

        // For zero based faders, fing the start position of the bar
        if (mode == Control::Mode::Default) {
            barX =
                map(std::max((int16_t)0, min), min, max, 0, bounds.getWidth());
        }

        uint16_t barWidth = map(val, min, max, 0, bounds.getWidth()) - barX;

        // Clear the component area
        g.fillAll(backgroundColour);

        // Paint the track background
        g.setColour(colourTrack);
        g.fillRect(0, padding, bounds.getWidth(), barHeight);

        // Paint the active bar
        g.setColour(colour);
        g.fillRect(barX, padding, barWidth, barHeight);

        if (barWidth > 1) {
            g.setColour(Colours::black);
            uint16_t separatorX = 0;
            separatorX = ((val > 0) ? barX + barWidth : barX + barWidth - 1);
            g.drawLine(
                separatorX, padding, separatorX, padding + barHeight - 1);
        }

        // Paint the outline
        g.setColour(backgroundColour);
        g.drawPixel(0, padding);
        g.drawPixel(0, padding + barHeight - 1);
        g.drawPixel(getWidth(), padding);
        g.drawPixel(getWidth(), padding + barHeight - 1);

        g.setColour(Colours::darker(colourTrack, 0.9f));
        g.drawPixel(1, padding);
        g.drawPixel(0, padding + 1);
        g.drawPixel(1, padding + barHeight - 1);
        g.drawPixel(0, padding + barHeight - 2);
        g.drawPixel(getWidth() - 2, padding);
        g.drawPixel(getWidth(), padding + 1);
        g.drawPixel(getWidth() - 2, padding + barHeight - 1);
        g.drawPixel(getWidth(), padding + barHeight - 2);

        if (variant == Control::Variant::Thin) {
            paintValueFixed(g, bounds, colour, val, items, mode, variant);
        } else {
            paintValueFloating(
                g, bounds, colour, barX + barWidth, val, items, mode, variant);
        }
    }

    void paintValueFixed(Graphics &g,
                         const Rectangle &bounds,
                         uint32_t colour,
                         int16_t val,
                         const ListData *items,
                         Control::Mode mode,
                         Control::Variant variant)
    {
        uint16_t labelYPosition = 0;

        // Print the label text if exists
        if (items && !items->getByValue(val).isLabelEmpty()) {
            g.setColour(Colours::white);
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
                        labelYPosition,
                        stringValue,
                        TextStyle::mediumTransparent,
                        getWidth(),
                        TextAlign::center);
        }
    }

    void paintValueFloating(Graphics &g,
                            const Rectangle &bounds,
                            uint32_t colour,
                            uint16_t x,
                            int16_t val,
                            const ListData *items,
                            Control::Mode mode,
                            Control::Variant variant)
    {
        uint16_t labelYPosition = 13;

        char stringValue[10];
        if (!control.getValue(0).getFormatter().empty()) {
            control.getValue(0).callFormatter(
                val, stringValue, sizeof(stringValue));
        } else {
            snprintf(stringValue, sizeof(stringValue), formatString, val);
        }
        uint16_t textWidth =
            TextBTE::getTextWidth(stringValue, TextStyle::mediumTransparent);

        if ((x + 5 + textWidth) < (getWidth() - 10)) {
            g.setColour(val >= 0 ? Colours::white : Colours::black);
            g.print(
                x + 5, labelYPosition, stringValue, textWidth, TextAlign::left);
        } else {
            g.setColour(val >= 0 ? Colours::black : Colours::white);
            g.print(x - 10 - textWidth,
                    labelYPosition,
                    stringValue,
                    textWidth,
                    TextAlign::right);
        }
    }

    struct {
        uint16_t previousScreenX : 10;
        bool thresholdCrossed : 1;
    };
};
