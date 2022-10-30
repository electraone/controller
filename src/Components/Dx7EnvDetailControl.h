#pragma once

#include "Dx7EnvControl.h"

class Dx7EnvDetailControl : public Dx7EnvControl
{
public:
    Dx7EnvDetailControl(const Control &control, MainDelegate &newDelegate)
        : Dx7EnvControl(control, newDelegate)
    {
    }

    virtual ~Dx7EnvDetailControl() = default;

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

    void paint(Graphics &g) override
    {
        Rectangle envBounds = getBounds();
        envBounds.setHeight(envBounds.getHeight());
        computePoints(envBounds);
        g.fillAll(LookAndFeel::backgroundColour);
        LookAndFeel::paintEnvelope(
            g, envBounds, control.getColour565(), baselineY, points, 0, false);
    }
};
