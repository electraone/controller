#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "ADR.h"

class ADRControl : public ControlComponent, public ADR
{
public:
    ADRControl(const Control &control, UiDelegate *newDelegate)
        : ControlComponent(control), delegate(newDelegate)
    {
        setMin(ADR::attack, control.values[0].getMin());
        setMax(ADR::attack, control.values[0].getMax());
        setMin(ADR::decay, control.values[1].getMin());
        setMax(ADR::decay, control.values[1].getMax());
        setMin(ADR::release, control.values[2].getMin());
        setMax(ADR::release, control.values[2].getMax());
        setActiveSegment(ADR::attack);
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
        updateValueFromParameterMap();
    }

    virtual ~ADRControl() = default;

    virtual void onTouchMove(const TouchEvent &touchEvent) override
    {
        uint8_t activeHandle = getActiveSegment();
        int16_t max = values[activeHandle].getMax();
        int16_t min = values[activeHandle].getMin();

        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        emitValueChange(newDisplayValue, control.getValue(activeHandle));
    }

    virtual void onPotTouchDown(const PotEvent &potEvent) override
    {
        showActiveSegment(true);
        delegate->setActivePotTouch(potEvent.getPotId(), this);
    }

    virtual void onPotChange(const PotEvent &potEvent) override
    {
        if (int16_t delta = potEvent.getAcceleratedChange()) {
            uint8_t activeHandle = getActiveSegment();
            int16_t newDisplayValue = getValue(activeHandle) + delta;
            emitValueChange(newDisplayValue, control.getValue(activeHandle));
        }
    }

    virtual void onPotTouchUp(const PotEvent &potEvent) override
    {
        showActiveSegment(false);
        delegate->resetActivePotTouch(potEvent.getPotId());
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

        if (0 <= handle && handle <= 2) {
            setValue(handle, newDisplayValue);
        }
    }

    void paint(Graphics &g) override
    {
        Rectangle envBounds = getBounds();
        envBounds.setHeight(envBounds.getHeight() / 2);
        computePoints(envBounds);
        LookAndFeel::paintEnvelope(g,
                                   envBounds,
                                   colour,
                                   baselineY,
                                   points,
                                   activeSegment,
                                   activeSegmentIsShown);

        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
    }

protected:
    UiDelegate *delegate;
};
