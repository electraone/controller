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
        updateValueFromParameterMap();
        enableEncoderMode(true);
    }

    virtual ~ListButtonControl() = default;

    virtual void onTouchDown(const TouchEvent &touchEvent) override
    {
        setActive(true);
    }

    virtual void onTouchUp(const TouchEvent &touchEvent) override
    {
        setActive(false);
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
        paintButtonList(
            g, bounds, control.getColour565(), getList(), index, getActive());
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
    void paintButtonList(Graphics &g,
                         const Rectangle &bounds,
                         uint32_t colour,
                         const ListData *items,
                         uint16_t activeIndex,
                         bool active)
    {
        if (active) {
            g.setColour(Colours565::darker(colour, 0.2f));
            g.fillRoundRect(0, 0, bounds.getWidth(), bounds.getHeight(), 5);
        }

        g.setColour(colour);
        g.drawRoundRect(0, 0, bounds.getWidth(), bounds.getHeight(), 5);

        if (items->getByIndex(activeIndex).isBitmapEmpty()) {
            char stringValue[20];
            if (!control.getValue(0).getFormatter().empty()) {
                control.getValue(0).callFormatter(
                    activeIndex, stringValue, sizeof(stringValue));
            } else {
                snprintf(stringValue,
                         sizeof(stringValue),
                         "%s",
                         items->getByIndex(activeIndex).getLabel());
            }
            // Print the label
            g.printText(0,
                        bounds.getHeight() * 0.3f,
                        stringValue,
                        TextStyle::mediumTransparent,
                        bounds.getWidth(),
                        TextAlign::center);
        } else {
            // display bitmap image
            uint16_t paddingBitmap =
                ((bounds.getWidth() - BITMAP_WIDTH)) / 2 - 1;
            items->getByIndex(activeIndex)
                .paintBitmap(paddingBitmap,
                             bounds.getHeight() * 0.3f,
                             Colours565::white);
        }
    }

    static Overlay empty;
};
