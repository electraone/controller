#pragma once

#include "Control.h"
#include "ControlComponent.h"

class RelativeControl : public ControlComponent
{
public:
    RelativeControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate), delta(0)
    {
    }

    virtual ~RelativeControl() = default;

    virtual void onPotTouchDown(const PotEvent &potEvent) override
    {
        delta = 0;
        ControlComponent::onPotTouchDown(potEvent);
    }

    virtual void onPotChange(const PotEvent &potEvent) override
    {
        delta = potEvent.getAcceleratedChange();
        emitValueChange(delta, control.getValue(0));
        repaint();
    }

    virtual void onPotTouchUp(const PotEvent &potEvent) override
    {
        delta = 0;
        ControlComponent::onPotTouchUp(potEvent);
    }

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) override
    {
    }

    void paint(Graphics &g) override
    {
        uint16_t width = getWidth();
        uint16_t centerX = width / 2;

        uint32_t colour = control.getColour565();
        uint32_t colourTrack = Colours565::darker(colour, 0.3f);
        uint32_t backgroundColour = getUseAltBackground()
                                        ? LookAndFeel::altBackgroundColour
                                        : LookAndFeel::backgroundColour;

        // Clear the component area
        g.fillAll(backgroundColour);

        char stringValue[20];
        if (!control.getValue(0).getFormatter().empty()) {
            control.getValue(0).callFormatter(
                delta, stringValue, sizeof(stringValue));
        } else {
            snprintf(stringValue, sizeof(stringValue), "%d", delta);
        }

        g.printText(0,
                    0,
                    stringValue,
                    TextStyle::mediumTransparent,
                    width,
                    TextAlign::center);

        for (uint8_t i = 0; i < 3; i++) {
            leftArrow(g,
                      (centerX - 42) + i * 10,
                      (delta < 0) && (abs(delta) > (2 - i)),
                      colour,
                      colourTrack);
        }
        for (uint8_t i = 0; i < 3; i++) {
            rightArrow(g,
                       (centerX + 42) - i * 10,
                       (delta > 0) && (delta > (2 - i)),
                       colour,
                       colourTrack);
        }

        g.setColour(getActive() ? colour : colourTrack);
        g.fillRect(centerX - padding, yPosition, padding * 2, 7);
        g.setColour(backgroundColour);
        g.drawPixel(centerX - padding, yPosition);
        g.drawPixel(centerX - padding, yPosition + 6);
        g.drawPixel(centerX + padding - 1, yPosition);
        g.drawPixel(centerX + padding - 1, yPosition + 6);

        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);

        ControlComponent::paint(g);
    }

private:
    int16_t delta;

    void leftArrow(Graphics &g,
                   uint16_t x,
                   bool active,
                   uint32_t colour,
                   uint32_t colourTrack)
    {
        g.setColour(active ? colour : colourTrack);
        g.fillTriangle(
            x + 0, yPosition + 3, x + 3, yPosition + 0, x + 3, yPosition + 6);
        g.fillRect(x + 4, yPosition + 0, 4, 7);
    }

    void rightArrow(Graphics &g,
                    uint16_t x,
                    bool active,
                    uint32_t colour,
                    uint32_t colourTrack)
    {
        g.setColour(active ? colour : colourTrack);
        g.fillTriangle(
            x - 1, yPosition + 3, x - 4, yPosition + 0, x - 4, yPosition + 6);
        g.fillRect(x - 8, yPosition + 0, 4, 7);
    }

    static constexpr uint16_t padding = 8;
    static constexpr uint16_t yPosition = 21;
};
