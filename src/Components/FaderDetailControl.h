#pragma once

#include "FaderControl.h"

class FaderDetailControl : public FaderControl
{
public:
    FaderDetailControl(const Control &control, MainDelegate &newDelegate)
        : FaderControl(control, newDelegate)
    {
        assignPot(control.inputs[0].getPotId(),
                  control.values[0].getNumSteps());
        setColour(control.getColour565());
    }

    virtual ~FaderDetailControl() = default;

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = value.getMax();
        int16_t min = value.getMin();
        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        emitValueChange(newDisplayValue, control.getValue(0));
    }

    void paint(Graphics &g)
    {
        const Rectangle &bounds = getBounds();
        int16_t val = value.get();
        int16_t min = value.getMin();
        int16_t max = value.getMax();
        Control::Mode mode = control.getMode();

        g.fillAll(Colours565::black);

        uint32_t colourTrack = Colours565::darker(colour, 0.3f);
        uint32_t backgroundColour = getUseAltBackground()
                                        ? LookAndFeel::altBackgroundColour
                                        : LookAndFeel::backgroundColour;

        uint16_t barHeight = bounds.getHeight();
        ;
        uint16_t padding = 0;
        uint16_t barX = 0;

        // For zero based faders, fing the start position of the bar
        if (mode == Control::Mode::Bipolar) {
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
            g.setColour(Colours565::black);
            uint16_t separatorX = 0;
            separatorX = ((val > 0) ? barX + barWidth : barX + barWidth - 1);
            g.drawLine(
                separatorX, padding, separatorX, padding + barHeight - 1);
        }
    }

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (!delegate.isDetailLocked()) {
            delegate.closeDetail();
        }
    }

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (!delegate.isDetailLocked()) {
            delegate.closeDetail();
        }
    }
};
