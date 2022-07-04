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
        uint16_t backgroundColour = getColour();
        uint16_t radius = getRadius();
        bool highlighted = isHighlighted();
        bool selected = isSelected();

        if (selected) {
            backgroundColour = getSelectedColour();
        }

        if (highlighted) {
            backgroundColour = Colours::lighter(backgroundColour, 0.1f);
        }

        g.setColour(Colours::darker(backgroundColour, 0.5f));
        g.fillRoundRect(0, 0, getWidth(), getHeight(), radius);
        g.setColour(backgroundColour);
        g.fillRoundRect(0, 0, getWidth(), getHeight() - 4, radius);
        g.printText(0,
                    getHeight() / 2 - 7,
                    getLabel(),
                    TextStyle::smallTransparent,
                    getWidth(),
                    TextAlign::center);
        if (dimmed == true) {
            g.dimAll();
        }
    }
};
