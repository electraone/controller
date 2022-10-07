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
        setColour(group.getColour());
        Set::paint(g);
    }

private:
    const Group &group;
    bool useAltBackground;
};
