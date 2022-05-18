#pragma once

#include "helpers.h"
#include "Colours.h"

class Group
{
private:
    static constexpr uint8_t maxLabelLength = 40;

public:
    Group() : id(0), pageId(0), colour(ElectraColours::white)
    {
        setLabel(nullptr);
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

    virtual ~Group() = default;

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
            copyString(label, newLabel, maxLabelLength);
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

    uint8_t getColour(void) const
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
    char label[maxLabelLength + 1];
    uint8_t pageId;
    uint8_t colour;
    Rectangle bounds;
};

typedef std::map<uint8_t, Group> Groups;
