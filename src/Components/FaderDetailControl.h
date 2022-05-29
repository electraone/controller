#pragma once

#include "FaderControl.h"

class FaderDetailControl : public FaderControl
{
public:
    explicit FaderDetailControl(const Control &control) : FaderControl(control)
    {
    }

    virtual ~FaderDetailControl() = default;

    void paint(Graphics &g)
    {
        g.fillAll(Colours::black);
        BarHorizontal::paint(g);
    }

private:
};
