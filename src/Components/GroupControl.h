#pragma once

#include "Group.h"
#include "Set.h"

class GroupControl : public Set
{
public:
    explicit GroupControl(const Group &groupToAssign) : group(groupToAssign)
    {
        setBounds(group.getBounds());
        setLabel(group.getLabel());
    }

    virtual ~GroupControl() = default;

    void paint(Graphics &g) override
    {
        setColour(ElectraColours::getNumericRgb565(group.getColour()));
        Set::paint(g);
    }

private:
    const Group &group;
};
