#pragma once

#include "Component.h"
#include "FaderControl.h"
#include "ListControl.h"
#include "PadControl.h"
#include "ADSRControl.h"
#include "ADRControl.h"
#include "DX7EnvControl.h"

class ControlComponent
{
public:
    virtual ~ControlComponent() = default;

    static Component *createControlComponent(const Control &control);

private:
};
