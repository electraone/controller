#pragma once

#include "Components/Button.h"

class ActionButton : public Button
{
public:
    ActionButton()
    {
    }

    virtual ~ActionButton() = default;

    void paint(Graphics &g) override
    {
        uint16_t backgroundColour = getColour565();
        uint16_t radius = getRadius();
        bool highlighted = isHighlighted();
        bool selected = isSelected();

        if (selected) {
            backgroundColour = getSelectedColour565();
        }

        if (highlighted) {
            backgroundColour = Colours565::lighter(backgroundColour, 0.1f);
        }

        g.setColour(Colours565::darker(backgroundColour, 0.5f));
        g.fillRoundRect(0, 0, getWidth(), getHeight(), radius);
        g.setColour(backgroundColour);
        g.fillRoundRect(0, 0, getWidth(), getHeight() - 4, radius);
        g.printText(0,
                    getHeight() / 2 - 7,
                    getLabel(),
                    TextStyle::smallTransparent,
                    getWidth(),
                    TextAlign::center);

        if (selected) {
            g.setColour(Colours565::lighter(backgroundColour, 0.1f));
            g.fillRoundRect(0, 0, 20, getHeight() - 4, radius);
            g.setColour(backgroundColour);
            g.fillRect(15, 0, 5, getHeight() - 4);
        }
        if (dimmed == true) {
            g.dimAll();
        }
    }
};
