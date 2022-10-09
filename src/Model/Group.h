#pragma once

#include "helpers.h"
#include "Colours.h"
#include "Component.h"

class Group
{
private:
    static constexpr uint8_t maxLabelLength = 40;

public:
    Group()
        : id(0),
          pageId(0),
          colour(Colours565::white),
          visible(true),
          component(nullptr)
    {
        setLabel(nullptr);
    }

    Group(uint8_t newId,
          uint8_t newPageId,
          const Rectangle &newBounds,
          const char *newLabel,
          uint32_t newColour)
        : id(newId),
          pageId(newPageId),
          colour(newColour),
          bounds(newBounds),
          visible(true),
          component(nullptr)
    {
        setLabel(newLabel);
    }

    virtual ~Group() = default;

    bool isValid(void) const
    {
        return (id != 0);
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

    void setColour(uint32_t newColour)
    {
        colour = newColour;
    }

    uint32_t getColour(void) const
    {
        return (colour);
    }

    uint16_t getColour565(void) const
    {
        return (Colours888::toRGB565(colour));
    }

    void setVisible(bool shouldBeVisible)
    {
        visible = shouldBeVisible;
    }

    bool isVisible(void) const
    {
        return ((bool)visible);
    }

    void setBounds(const Rectangle &newBounds)
    {
        bounds = newBounds;
    }

    Rectangle getBounds(void) const
    {
        return (bounds);
    }

    void setComponent(Component *newComponent)
    {
        component = newComponent;
    }

    void resetComponent(void)
    {
        component = nullptr;
    }

    Component *getComponent(void) const
    {
        return (component);
    }

    void print(void) const
    {
        logMessage("id: %d", getId());
        logMessage("label: %s", getLabel());
        logMessage("pageId: %d", getPageId());
        logMessage("colour: %d", getColour());
        logMessage("visible: %d", isVisible());
        getBounds().print();
    }

private:
    struct {
        uint16_t id : 9;
        uint8_t pageId : 4;
        uint8_t visible : 1;
        uint32_t colour : 24;
    };

    char label[maxLabelLength + 1];
    Rectangle bounds;

    Component *component;
};

typedef std::map<uint8_t, Group> Groups;
