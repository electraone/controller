#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "ADSR.h"

class ADSRControl : public ControlComponent, public ADSR
{
public:
    ADSRControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate)
    {
        setMin(ADSR::attack, control.values[0].getMin());
        setMax(ADSR::attack, control.values[0].getMax());
        setMin(ADSR::decay, control.values[1].getMin());
        setMax(ADSR::decay, control.values[1].getMax());
        setMin(ADSR::sustain, control.values[2].getMin());
        setMax(ADSR::sustain, control.values[2].getMax());
        setMin(ADSR::release, control.values[3].getMin());
        setMax(ADSR::release, control.values[3].getMax());
        setActiveSegment(control.inputs[0].getValueId());
        updateValueFromParameterMap();
    }

    virtual ~ADSRControl() = default;

    virtual void onTouchDown(const TouchEvent &touchEvent) override
    {
        previousScreenX = touchEvent.getScreenX();
    }

    virtual void onTouchMove(const TouchEvent &touchEvent) override
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

    virtual void onPotTouchDown(const PotEvent &potEvent) override
    {
        showActiveSegment(true);
        ControlComponent::onPotTouchDown(potEvent);
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
        ControlComponent::onPotTouchUp(potEvent);
    }

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) override
    {
        int16_t newDisplayValue = value.translateMidiValue(midiValue);
        if (0 <= handle && handle <= 3) {
            setValue(handle, newDisplayValue);
        }
    }

    virtual void paint(Graphics &g) override
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
    uint16_t previousScreenX;
};
