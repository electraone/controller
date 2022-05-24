#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "ADR.h"

class ADRControl : public ControlComponent, public ADR
{
public:
    explicit ADRControl(const Control &control)
        : ControlComponent(control), activeHandle(1)
    {
        setMin(ADR::attack, control.values[0].getMin());
        setMax(ADR::attack, control.values[0].getMax());
        setValue(ADR::attack, control.values[0].getDefault());

        setMin(ADR::decay, control.values[1].getMin());
        setMax(ADR::decay, control.values[1].getMax());
        setValue(ADR::decay, control.values[1].getDefault());

        setMin(ADR::release, control.values[2].getMin());
        setMax(ADR::release, control.values[2].getMax());
        setValue(ADR::release, control.values[2].getDefault());

        setActiveSegment(ADR::attack);

        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~ADRControl() = default;

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        int16_t max = values[activeHandle].getMax();
        int16_t min = values[activeHandle].getMin();

        float step = getWidth() / (float)(max - min);
        int16_t newDisplayValue =
            constrain(ceil(touchEvent.getX() / step + min), min, max);

        uint16_t midiValue = translateValueToMidiValue(
            control.values[activeHandle - 1].message.getSignMode(),
            control.values[activeHandle - 1].message.getBitWidth(),
            newDisplayValue,
            control.values[activeHandle - 1].getMin(),
            control.values[activeHandle - 1].getMax(),
            control.values[activeHandle - 1].message.getMidiMin(),
            control.values[activeHandle - 1].message.getMidiMax());

        parameterMap.setValue(
            control.values[activeHandle - 1].message.getDeviceId(),
            control.values[activeHandle - 1].message.getType(),
            control.values[activeHandle - 1].message.getParameterNumber(),
            midiValue,
            Origin::internal);

#ifdef DEBUG
        logMessage(
            "onTouchMove: display=%d, midi=%d", newDisplayValue, midiValue);
#endif
    }

    void onPotChange(const PotEvent &potEvent) override
    {
        if (potEvent.getRelativeChange()) {
            int16_t delta = potEvent.getAcceleratedChange();
            int16_t newDisplayValue = getValue(activeHandle) + delta;

            uint16_t midiValue = translateValueToMidiValue(
                control.values[activeHandle - 1].message.getSignMode(),
                control.values[activeHandle - 1].message.getBitWidth(),
                newDisplayValue,
                control.values[activeHandle - 1].getMin(),
                control.values[activeHandle - 1].getMax(),
                control.values[activeHandle - 1].message.getMidiMin(),
                control.values[activeHandle - 1].message.getMidiMax());

            parameterMap.setValue(
                control.values[activeHandle - 1].message.getDeviceId(),
                control.values[activeHandle - 1].message.getType(),
                control.values[activeHandle - 1].message.getParameterNumber(),
                midiValue,
                Origin::internal);

#ifdef DEBUG
            logMessage("onPotChange: display=%d, midi=%d, delta=%d",
                       newDisplayValue,
                       midiValue,
                       potEvent.getRelativeChange());
#endif
        }
    }

    void onMidiValueChange(const ControlValue &value,
                           int16_t midiValue,
                           uint8_t handle = 1) override
    {
        int16_t newDisplayValue =
            translateMidiValueToValue(value.message.getSignMode(),
                                      value.message.getBitWidth(),
                                      midiValue,
                                      value.message.getMidiMin(),
                                      value.message.getMidiMax(),
                                      value.getMin(),
                                      value.getMax());

        // The if construct is left here on purpose as mapping of handles
        // to envelope segments is not always 1:1
        if (handle == ADR::attack) {
            setValue(ADR::attack, newDisplayValue);
        } else if (handle == ADR::decay) {
            setValue(ADR::decay, newDisplayValue);
        } else if (handle == 4) { // a compatibility thing \todo resolve that
            setValue(ADR::release, newDisplayValue);
        }
    }

    void paint(Graphics &g) override
    {
        Rectangle envBounds = getBounds();
        envBounds.setHeight(envBounds.getHeight() / 2);
        computePoints(envBounds);
        LookAndFeel::paintEnvelope(g, envBounds, colour, baselineY, points);

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
