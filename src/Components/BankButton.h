#pragma once

#include "Components/Button.h"

class BankButton : public Button
{
public:
    BankButton()
    {
    }

    virtual ~BankButton() = default;

    void paint(Graphics &g) override
    {
        uint16_t backgroundColour = getColour();

        g.fillAll(LookAndFeel::backgroundColour);

        if (isSelected()) {
            g.setColour(backgroundColour);
            g.fillRoundRect(0, 0, getWidth(), getHeight(), getRadius());
            g.dimAll();
        } else if (isHighlighted()) {
            backgroundColour = Colours::lighter(backgroundColour, 0.1f);
            g.setColour(backgroundColour);
            g.fillRoundRect(0, 0, getWidth(), getHeight(), getRadius());
            g.dimAll();
        }

        g.setColour(getSelectedColour());
        g.drawRoundRect(0, 0, getWidth(), getHeight(), getRadius());
        g.printText(0,
                    getHeight() / 2 - 7,
                    getLabel(),
                    TextStyle::smallTransparent,
                    getWidth(),
                    TextAlign::center);
    }
};
