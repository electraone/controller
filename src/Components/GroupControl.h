#pragma once

#include "Control.h"
#include "Set.h"

class GroupControl : public Set
{
public:
    explicit GroupControl(const Group &group)
    {
		setBounds(group.getBounds());
		setLabel(group.getLabel());
        setColour(ElectraColours::getNumericRgb565(group.getColour()));
		setColour(Colours::white);
    }

    virtual ~GroupControl() = default;

private:
};
