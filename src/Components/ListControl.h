#pragma once

#include "Control.h"
#include "ControlComponent.h"
#include "List.h"

class ListControl : public ControlComponent, public List
{
public:
    static constexpr uint16_t thresholdSwipeDistance = 40;

    ListControl(const Control &control, MainDelegate &newDelegate)
        : ControlComponent(control, newDelegate),
          List(control.values[0].getOverlay())
    {
        setColour(ElectraColours::getNumericRgb565(control.getColour()));
        updateValueFromParameterMap();
        enableEncoderMode(true);
    }

    virtual ~ListControl() = default;

    virtual void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (auto list = getList()) {
            int16_t delta = touchEvent.getTouchDownX() - touchEvent.getX();

            if (abs(delta) > thresholdSwipeDistance) {
                int8_t step = (delta < 0) - (delta > 0);
                uint16_t newIndex =
                    constrain(index + step, 0, list->getMaxIndex());

                emitValueChange(newIndex, control.getValue(0));
            }
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
                int16_t newIndex =
                    constrain(index + delta, 0, list->getMaxIndex());
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
        Rectangle listBounds = getBounds();
        listBounds.setHeight(listBounds.getHeight() / 2);
        g.fillAll(getUseAltBackground() ? LookAndFeel::altBackgroundColour
                                        : LookAndFeel::backgroundColour);
        LookAndFeel::paintList(g, listBounds, colour, getList(), index);
        g.printText(0,
                    getHeight() - 20,
                    getName(),
                    TextStyle::mediumWhiteOnBlack,
                    getWidth(),
                    TextAlign::center,
                    2);
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
