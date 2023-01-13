#pragma once

#include "Colours.h"
#include "Component.h"

class Group
{
private:
    static constexpr uint8_t maxLabelLength = 40;

public:
    enum class Variant { Default = 0, Highlighted = 1 };

    Group()
        : id(0),
          pageId(0),
          colour(Colours565::white),
          visible(true),
          component(nullptr)
    {
        setLabel(nullptr);
        variant = (uint8_t)Variant::Default;
    }

    Group(uint16_t newId,
          uint8_t newPageId,
          const Rectangle &newBounds,
          const char *newLabel,
          uint32_t newColour,
          Group::Variant newVariant)
        : id(newId),
          pageId(newPageId),
          colour(newColour),
          bounds(newBounds),
          visible(true),
          component(nullptr)
    {
        setLabel(newLabel);
        variant = (uint8_t)newVariant;
    }

    virtual ~Group() = default;

    bool isValid(void) const
    {
        return (id != 0);
    }

    void setId(uint16_t newId)
    {
        id = newId;
    }

    uint16_t getId(void) const
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

    void setVariant(Variant newVariant)
    {
        variant = (uint8_t)newVariant;
    }

    Variant getVariant(void) const
    {
        return ((Variant)variant);
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
        System::logger.write("id: %d", getId());
        System::logger.write("label: %s", getLabel());
        System::logger.write("pageId: %d", getPageId());
        System::logger.write("colour: %d", getColour());
        System::logger.write("visible: %d", isVisible());
        System::logger.write("variant: %d", getVariant());
        getBounds().print();
    }

    static Variant translateVariant(const char *variantText)
    {
        if (variantText) {
            if (strcmp(variantText, "highlighted") == 0) {
                return (Variant::Highlighted);
            }
        }
        return (Variant::Default);
    }

private:
    struct {
        uint16_t id : 10;
        uint8_t pageId : 4;
        uint8_t visible : 1;
        uint32_t colour : 24;
        uint8_t variant : 2;
    };

    char label[maxLabelLength + 1];
    Rectangle bounds;

    Component *component;
};

typedef std::map<uint16_t, Group> Groups;
