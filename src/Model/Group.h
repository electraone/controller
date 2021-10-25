#pragma once

#include "helpers.h"
#include "Colours.h"

class Group
{
private:
    static const uint8_t maxGroupNameLength = 40;

public:
    Group() : id(0), pageId(0), colour(ElectraColours::white)
    {
        *label = '\0';
    }

    Group(uint8_t newId,
          uint8_t newPageId,
          const Rectangle &newBounds,
          const char *newLabel,
          Colour newColour)
        : id(newId), pageId(newPageId), colour(newColour), bounds(newBounds)
    {
        setLabel(newLabel);
    }

    void setId(uint8_t newId)
    {
        id = newId;
    }

    uint8_t getId(void) const
    {
        return (id);
    }

    void setLabel(const char *newLabel)
    {
        if (newLabel) {
            copyString(label, newLabel, maxGroupNameLength);
        } else {
            *label = '\0';
        }
    }

    const char *getLabel(void) const
    {
        return (label);
    }

    void setPageId(uint8_t newPageId)
    {
        pageId = newPageId;
    }

    uint8_t getPageId(void) const
    {
        return (pageId);
    }

    void setColour(Colour newColour)
    {
        colour = newColour;
    }

    Colour getColour(void) const
    {
        return (colour);
    }

    void setBounds(const Rectangle &newBounds)
    {
        bounds = newBounds;
    }

    Rectangle getBounds(void) const
    {
        return (bounds);
    }

	void print(void) const
	{
	    logMessage("id: %d", getId());
	    logMessage("label: %s", getLabel());
	    logMessage("pageId: %d", getPageId());
	    logMessage("colour: %d", getColour());
	    getBounds().print();
	}

private:
    uint8_t id;
    char label[maxGroupNameLength + 1];
    uint8_t pageId;
    Colour colour;
    Rectangle bounds;
};
