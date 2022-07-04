#pragma once

#include "Components/Button.h"

class PresetButton : public Button
{
public:
    PresetButton()
    {
        setColours(0x2820, 0x4860);
    }

    virtual ~PresetButton() = default;
};
