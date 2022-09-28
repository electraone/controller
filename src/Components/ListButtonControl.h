#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "List.h"

class ListButtonControl : public ControlComponent, public List
{
public:
    static constexpr uint16_t thresholdSwipeDistance = 40;

    ListButtonControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate),
          List(control.values[0].getOverlay())
    {
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
        updateValueFromParameterMap();
        enableEncoderMode(true);
    }

    virtual ~ListButtonControl() = default;

    virtual void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (auto list = getList()) {
            uint16_t newIndex = index + 1;
            if (newIndex > list->getMaxIndex()) {
                newIndex = 0;
            }
            emitValueChange(newIndex, control.getValue(0));
        }
    }

    virtual void onPotTouchDown(const PotEvent &potEvent) override
    {
        ControlComponent::onPotTouchDown(potEvent);
    }

    virtual void onPotChange(const PotEvent &potEvent) override
    {
        if (auto list = getList()) {
            if (int16_t delta = potEvent.getAcceleratedChange()) {
                int16_t newIndex = index + delta;

                if (newIndex > list->getMaxIndex()) {
                    newIndex = 0;
                } else if (newIndex < 0) {
                    newIndex = list->getMaxIndex();
                }
                emitValueChange(newIndex, control.getValue(0));
            }
        }
    }

    virtual void onPotTouchUp(const PotEvent &potEvent) override
    {
        ControlComponent::onPotTouchUp(potEvent);
    }

    virtual void onMidiValueChange(const ControlValue &value,
                                   int16_t midiValue,
                                   uint8_t handle = 0) override
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
        auto bounds = getBounds();
        bounds.setX(0);
        bounds.setY(3);
        bounds.setWidth(getWidth());
        bounds.setHeight(getHeight() - 6);
        g.fillAll(getUseAltBackground() ? LookAndFeel::altBackgroundColour
                                        : LookAndFeel::backgroundColour);
        LookAndFeel::paintButtonList(g, bounds, colour, getList(), index);
        ControlComponent::paint(g);
    }

    void emitValueChange(int16_t newIndex, const ControlValue &cv)
    {
        if (auto list = getList()) {
            int16_t midiValue = list->getValueByIndex(newIndex);

            parameterMap.setValue(
                control.values[0].message.getDeviceId(),
                control.values[0].message.getType(),
                control.values[0].message.getParameterNumber(),
                midiValue,
                Origin::internal);

            cv.callFunction(newIndex);
        }
    }

private:
    static Overlay empty;
};
