#pragma once

#include "Components/Button.h"

class BankButton : public Button
{
public:
    BankButton()
    {
    }

    void paint(Graphics &g) override
    {
        Button::paint(g);

        if (isSelected()) {
            g.setColour(Colours::lighter(getSelectedColour(), 0.3f));
            g.drawRoundRect(0, 0, getWidth(), getHeight(), getRadius());
        }
    }

    virtual ~BankButton() = default;
};
