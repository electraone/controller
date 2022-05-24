#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "List.h"

class ListControl : public ControlComponent, public List
{
public:
    static constexpr uint16_t thresholdSwipeDistance = 40;

    explicit ListControl(const Control &control)
        : ControlComponent(control), List(control.values[0].getOverlay())
    {
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
    }

    virtual ~ListControl() = default;

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (auto list = getList()) {
            int16_t delta = touchEvent.getTouchDownX() - touchEvent.getX();

            if (abs(delta) > thresholdSwipeDistance) {
                int8_t step = (delta < 0) - (delta > 0);
                uint16_t newIndex =
                    constrain(index + step, 0, list->getMaxIndex());

                int16_t midiValue = list->getValueByIndex(newIndex);

                parameterMap.setValue(
                    control.values[0].message.getDeviceId(),
                    control.values[0].message.getType(),
                    control.values[0].message.getParameterNumber(),
                    midiValue,
                    Origin::internal);

#ifdef DEBUG
                logMessage(
                    "onTouchMove: newIndex=%d, midiValue=%d, step=%d, delta=%d",
                    newIndex,
                    midiValue,
                    step,
                    delta);
#endif
            }
        }
    }

    void onPotChange(const PotEvent &potEvent) override
    {
        if (auto list = getList()) {
            int16_t newIndex =
                constrain(index + potEvent.getAcceleratedChange(),
                          0,
                          list->getMaxIndex());
            int16_t midiValue = list->getValueByIndex(newIndex);

            parameterMap.setValue(
                control.values[0].message.getDeviceId(),
                control.values[0].message.getType(),
                control.values[0].message.getParameterNumber(),
                midiValue,
                Origin::internal);

#ifdef DEBUG
            logMessage(
                "onPotChange: newIndex=%d, midiValue=%d", newIndex, midiValue);
#endif
        }
    }

    void onMidiValueChange(const ControlValue &value,
                           int16_t midiValue,
                           uint8_t handle = 1) override
    {
        if (auto list = getList()) {
            int16_t index = list->getIndexByValue(midiValue);

            if (index >= 0) {
                setIndex(index);
            }
        }
    }

    void paint(Graphics &g) override
    {
        Rectangle sliderBounds = getBounds();
        sliderBounds.setHeight(sliderBounds.getHeight() / 2);
        LookAndFeel::paintList(g, sliderBounds, colour, getList(), index);
        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
    }

private:
    static Overlay empty;
};
