#pragma once

#include "Detail.h"

class DetailEnvelope : public Detail
{
public:
    DetailEnvelope(const Control &control, UiDelegate *newDelegate)
        : Detail(control, newDelegate)
    {
        Detail::setLocked();
    }

    virtual ~DetailEnvelope() = default;

    void paint(Graphics &g)
    {
        g.setColour(Colours::black);
        g.fillRect(0, 0, getWidth(), getHeight() - 120);
        g.setColour(Colours::darkgrey);
        g.drawRect(0, 0, getWidth(), getHeight() - 120);
        g.dim(0, getHeight() - 120, getWidth(), 120, Colours::black);
    }

    void resized(void)
    {
        component->setBounds(50, 50, 808, 278);
        lock->setBounds(58, getHeight() - 100, 80, 80);
    }

private:
};
