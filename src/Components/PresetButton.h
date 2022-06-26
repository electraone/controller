#pragma once

#include "Components/Button.h"

class PresetButton : public Button
{
public:
    PresetButton()
    {
        setColours(0x0840, 0x18c0);
    }

    virtual ~PresetButton() = default;
};
