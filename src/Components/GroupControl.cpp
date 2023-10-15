/*
* Electra One MIDI Controller Firmware
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Electra One Project (http://electra.one/).
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.
*/

#include "GroupControl.h"

GroupControl::GroupControl(const Group &groupToAssign)
    : group(groupToAssign), useAltBackground(false)
{
    auto bounds = group.getBounds();
    setBounds(bounds);

    setLabel(group.getLabel());
    setName(group.getLabel());
    setHighlighted(group.getVariant() == Group::Variant::Highlighted);
    setColour(group.getColour565());
    setVisible(group.isVisible());
}

void GroupControl::setUseAltBackground(bool shouldUseAltBackground)
{
    useAltBackground = shouldUseAltBackground;
}

bool GroupControl::getUseAltBackground(void) const
{
    return (useAltBackground);
}

void GroupControl::paint(Graphics &g)
{
    Set::paint(g);
}
