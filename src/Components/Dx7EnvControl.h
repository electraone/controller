#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "Env5Seg.h"

class Dx7EnvControl : public ControlComponent, public Env5Seg
{
public:
    Dx7EnvControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate)
    {
        setValueRanges();
        setActiveSegment(control.inputs[0].getValueId());
        updateValueFromParameterMap();
    }

    virtual ~Dx7EnvControl() = default;

    void syncComponentProperties(void)
    {
        setValueRanges();
        ControlComponent::syncComponentProperties();
    }

    virtual void onTouchDown(const TouchEvent &touchEvent) override
    {
        previousScreenX = touchEvent.getScreenX();
    }

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        /* commented out on purpose
        uint8_t activeHandle = getActiveSegment();
        int16_t max = values[activeHandle].getMax();
        int16_t min = values[activeHandle].getMin();
        float step = getWidth() / (float)(max - min);
        int16_t delta = touchEvent.getScreenX() - previousScreenX;
        previousScreenX = touchEvent.getScreenX();

        int16_t newDisplayValue = constrain(
            ceil(getValue(activeHandle) + ((float)delta / step)), min, max);

        emitValueChange(newDisplayValue, control.getValue(activeHandle));
        */
    }

    void onPotTouchDown(const PotEvent &potEvent) override
    {
        showActiveSegment(true);
        ControlComponent::onPotTouchDown(potEvent);
    }

    void onPotChange(const PotEvent &potEvent) override
    {
        if (int16_t delta = potEvent.getAcceleratedChange()) {
            uint8_t activeHandle = getActiveSegment();
            int16_t newDisplayValue = getValue(activeHandle) + delta;
            emitValueChange(newDisplayValue, control.getValue(activeHandle));
        }
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        showActiveSegment(false);
        ControlComponent::onPotTouchUp(potEvent);
    }

    void onMidiValueChange(const ControlValue &value,
                           int16_t midiValue,
                           uint8_t handle = 0) override
    {
        int16_t newDisplayValue = value.translateMidiValue(midiValue);
        if (0 <= handle && handle <= 7) {
            if (handle % 2) {
                newDisplayValue = value.getMax() - newDisplayValue;
            }
            setValue(handle, newDisplayValue);
        }
    }

    void paint(Graphics &g) override
    {
        Rectangle envBounds = getBounds();
        envBounds.setHeight(envBounds.getHeight() / 2);
        computePoints(envBounds);
        g.fillAll(getUseAltBackground() ? LookAndFeel::altBackgroundColour
                                        : LookAndFeel::backgroundColour);
        LookAndFeel::paintEnvelope(g,
                                   envBounds,
                                   control.getColour565(),
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
        ControlComponent::paint(g);
    }

private:
    void setValueRanges(void)
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
    }

    uint16_t previousScreenX;
};
