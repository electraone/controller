#pragma once

#include "Group.h"
#include "Set.h"

class GroupControl : public Set
{
public:
    explicit GroupControl(const Group &groupToAssign)
        : group(groupToAssign), useAltBackground(false)
    {
        // tweak group's position. far from optimal,
        auto bounds = group.getBounds();
        bounds.setY(bounds.getY() * 1.02f - 5);
        setBounds(bounds);

        setLabel(group.getLabel());
        setName(group.getLabel());
        setHighlighted(group.getVariant() == Group::Variant::Highlighted);
        setVisible(group.isVisible());
    }

    virtual ~GroupControl() = default;

    void setUseAltBackground(bool shouldUseAltBackground)
    {
        useAltBackground = shouldUseAltBackground;
    }

    bool getUseAltBackground(void) const
    {
        return (useAltBackground);
    }

    void paint(Graphics &g) override
    {
        g.fillAll(useAltBackground ? LookAndFeel::altBackgroundColour
                                   : LookAndFeel::backgroundColour);
        colour = group.getColour565();
        Set::paint(g);
        //g.setColour(0x2258);
        //g.drawRect(0, 0, getWidth(), getHeight());
    }

private:
    const Group &group;
    bool useAltBackground;
};
