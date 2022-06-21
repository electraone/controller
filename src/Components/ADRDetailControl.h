#pragma once

#include "ADRControl.h"

class ADRDetailControl : public ADRControl
{
public:
    ADRDetailControl(const Control &control, UiDelegate *newDelegate)
        : ADRControl(control, newDelegate)
    {
    }

    virtual ~ADRDetailControl() = default;

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (!delegate->isDetailLocked()) {
            delegate->closeDetail();
        }
    }

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (!delegate->isDetailLocked()) {
            delegate->closeDetail();
        }
    }

    void paint(Graphics &g) override
    {
        Rectangle envBounds = getBounds();
        envBounds.setHeight(envBounds.getHeight());
        computePoints(envBounds);
        LookAndFeel::paintEnvelope(g,
                                   envBounds,
                                   colour,
                                   baselineY,
                                   points,
                                   activeSegment,
                                   activeSegmentIsShown);
    }
};
