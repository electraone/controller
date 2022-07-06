#pragma once

#include "ADSRControl.h"

class ADSRDetailControl : public ADSRControl
{
public:
    ADSRDetailControl(const Control &control, MainDelegate &newDelegate)
        : ADSRControl(control, newDelegate)
    {
    }

    virtual ~ADSRDetailControl() = default;

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (!delegate.isDetailLocked()) {
            delegate.closeDetail();
        }
    }

    void onTouchLongHold(const TouchEvent &touchEvent) override
    {
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (!delegate.isDetailLocked()) {
            delegate.closeDetail();
        }
    }

    virtual void paint(Graphics &g) override
    {
        Rectangle envBounds = getBounds();
        envBounds.setHeight(envBounds.getHeight());
        computePoints(envBounds);
        g.fillAll(LookAndFeel::backgroundColour);
        LookAndFeel::paintEnvelope(g,
                                   envBounds,
                                   colour,
                                   baselineY,
                                   points,
                                   activeSegment,
                                   activeSegmentIsShown);
    }
};
