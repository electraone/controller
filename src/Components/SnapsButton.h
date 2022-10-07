#pragma once

#include "Components/Button.h"

class SnapsButton : public Button
{
public:
    SnapsButton() : used(false)
    {
    }

    virtual ~SnapsButton() = default;

    void setUsed(bool shouldBeUsed)
    {
        used = shouldBeUsed;
    }

    bool isUsed(void)
    {
        return (used);
    }

    void setProgramInfo(const char *newProgramInfo)
    {
        copyString(programInfo, newProgramInfo, maxProgramInfoLength);
    }

    void setBankNumber(uint8_t newBankNumber)
    {
        bankNumber = newBankNumber;
    }

    void paint(Graphics &g)
    {
        if (used) {
            uint16_t textOffsetY = (getHeight() / 2) - 2;
            uint32_t colour = getColour();

            if (isHighlighted()) {
                colour = Colours::lighter(colour, 0.1f);
            }

            g.setColour(colour);
            g.fillRoundRect(0, 0, getWidth(), getHeight(), 4);
            g.printText(0,
                        textOffsetY,
                        getLabel(),
                        TextStyle::mediumTransparent,
                        getWidth(),
                        TextAlign::center);
            g.printText(5,
                        5,
                        programInfo,
                        TextStyle::smallTransparent,
                        40,
                        TextAlign::left);
        } else {
            g.setColour(Colours::black);
            g.fillRoundRect(0, 0, getWidth(), getHeight(), 4);

            g.setColour(0x2104);
            for (uint16_t i = 4; i < getWidth() - 4; i += 4) {
                g.drawPixel(i, 0);
                g.drawPixel(i, getHeight());
            }
            for (uint16_t i = 5; i < getHeight() - 4; i += 4) {
                g.drawPixel(0, i);
                g.drawPixel(getWidth(), i);
            }
            g.drawPixel(1, 1);
            g.drawPixel(1, getHeight() - 2);
            g.drawPixel(getWidth() - 2, 1);
            g.drawPixel(getWidth() - 2, getHeight() - 2);

            g.printText(5,
                        5,
                        programInfo,
                        TextStyle::smallTransparent,
                        40,
                        TextAlign::left);
            g.dim(5, 5, 40, 12, COLOR_BLACK);
        }
    }

    static const uint8_t maxLabelLength = Component::MaxNameLength;
    static const uint8_t maxProgramInfoLength = 7;

private:
    char programInfo[maxProgramInfoLength + 1];

    struct {
        uint8_t bankNumber : 4;
        bool used : 1;
    };
};
