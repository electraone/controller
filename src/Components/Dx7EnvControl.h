#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "Env5Seg.h"

class Dx7EnvControl : public ControlComponent, public Env5Seg
{
public:
    Dx7EnvControl(const Control &control, UiDelegate *newDelegate)
        : ControlComponent(control, newDelegate), activeHandle(0)
    {
        values[Env5Seg::level1].setMin(control.values[0].getMin());
        values[Env5Seg::level1].setMax(control.values[0].getMax());
        values[Env5Seg::rate1].setMin(control.values[1].getMin());
        values[Env5Seg::rate1].setMax(control.values[1].getMax());
        values[Env5Seg::level2].setMin(control.values[2].getMin());
        values[Env5Seg::level2].setMax(control.values[2].getMax());
        values[Env5Seg::rate2].setMin(control.values[3].getMin());
        values[Env5Seg::rate2].setMax(control.values[3].getMax());
        values[Env5Seg::level3].setMin(control.values[4].getMin());
        values[Env5Seg::level3].setMax(control.values[4].getMax());
        values[Env5Seg::rate3].setMin(control.values[5].getMin());
        values[Env5Seg::rate3].setMax(control.values[5].getMax());
        values[Env5Seg::level4].setMin(control.values[6].getMin());
        values[Env5Seg::level4].setMax(control.values[6].getMax());
        values[Env5Seg::rate4].setMin(control.values[7].getMin());
        values[Env5Seg::rate4].setMax(control.values[7].getMax());

        setActiveSegment(Env5Seg::level4);

        setColour(ElectraColours::getNumericRgb565(control.getColour()));
        updateValueFromParameterMap();
    }

    virtual ~Dx7EnvControl() = default;

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = values[activeHandle].getMax();
        int16_t min = values[activeHandle].getMin();

        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        emitValueChange(newDisplayValue, control.getValue(activeHandle));
    }

    void onPotTouchDown(const PotEvent &potEvent) override
    {
        showActiveSegment(true);
        delegate->setActivePotTouch(potEvent.getPotId(), this);
    }

    void onPotChange(const PotEvent &potEvent) override
    {
        if (int16_t delta = potEvent.getAcceleratedChange()) {
            int16_t newDisplayValue = getValue(activeHandle) + delta;
            emitValueChange(newDisplayValue, control.getValue(activeHandle));
        }
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        showActiveSegment(false);
        delegate->resetActivePotTouch(potEvent.getPotId());
    }

    void onMidiValueChange(const ControlValue &value,
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

        if (0 <= handle && handle <= 7) {
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
                                   activeHandle,
                                   activeSegmentIsShown);

        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
    }

private:
    uint8_t activeHandle;
};
