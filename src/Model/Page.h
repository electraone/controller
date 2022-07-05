#pragma once

#include "helpers.h"

class Page
{
public:
    Page() : id(0), hasObjects(false)
    {
        *name = '\0';
    }

    Page(uint8_t id, const char *newName, uint8_t newDefaultControlSetId)
        : id(id), defaultControlSetId(newDefaultControlSetId)
    {
        setName(newName);
    }

    virtual ~Page() = default;

    bool isValid(void) const
    {
        return (0 < id && id <= 12);
    }

    uint8_t getId(void) const
    {
        return (id);
    }

    void setName(const char *newName)
    {
        if (newName) {
            copyString(name, newName, MaxNameLength);
        } else {
            *name = '\0';
        }
    }

    const char *getName(void) const
    {
        return (name);
    }

    void setDefaultControlSetId(uint8_t newDefaultControlSetId)
    {
        defaultControlSetId = newDefaultControlSetId;
    }

    uint8_t getDefaultControlSetId(void) const
    {
        return (defaultControlSetId);
    }

    bool getHasObjects(void) const
    {
        return (hasObjects);
    }

    void setHasObjects(bool shouldHaveObjects)
    {
        hasObjects = shouldHaveObjects;
    }

    void print(void) const
    {
        logMessage("id: %d", getId());
        logMessage("name: %s", getName());
        logMessage("hasObjects: %d", getHasObjects());
    }

private:
    static constexpr uint8_t MaxNameLength = 20;

    uint8_t id;
    char name[MaxNameLength + 1];
    uint8_t defaultControlSetId;
    bool hasObjects;
};

typedef std::map<uint8_t, Page> Pages;
