#pragma once

#include "Group.h"
#include "Set.h"

class GroupControl : public Set
{
public:
    explicit GroupControl(const Group &groupToAssign) : group(groupToAssign)
    {
        auto bounds = group.getBounds();
        bounds.setY(bounds.getY() + 4);
        setBounds(bounds);
        setLabel(group.getLabel());
        setVisible(group.isVisible());
    }

    virtual ~GroupControl() = default;

    void paint(Graphics &g) override
    {
        g.fillAll(LookAndFeel::backgroundColour);
        setColour(ElectraColours::getNumericRgb565(group.getColour()));
        Set::paint(g);
    }

private:
    const Group &group;
};
