#pragma once

#include "Preset.h"

class UiDelegate
{
public:
    /*
     * Actions
     */

    // Window state management
    virtual void setPage(uint8_t pageId, uint8_t controlSetId) = 0;
    virtual void setControlSet(uint8_t controlSetId) = 0;
    virtual void openDetail(uint16_t controlId) = 0;
    virtual void lockDetail(void) = 0;
    virtual void closeDetail(void) = 0;
    virtual void openPageSelection(void) = 0;
    virtual void closePageSelection(void) = 0;
    virtual void repaintPage(void) = 0;
    virtual void repaintControl(uint16_t controlId) = 0;

    // Controls
    virtual void setControlVisible(Control *control, bool shouldBeVisible) = 0;
    virtual void setControlName(Control *control, const char *newName) = 0;
    virtual void setControlColour(Control *control, uint8_t newColour) = 0;
    virtual void setControlPot(Control *control,
                               uint8_t newControlSetId,
                               uint8_t newPotId) = 0;
    virtual void setControlBounds(Control *control,
                                  const Rectangle &bounds) = 0;
    virtual void setControlSlot(Control *control, uint8_t newSlot) = 0;

    // Pages
    virtual void setPageName(Page *page, const char *newName) = 0;

    // Groups
    virtual void setGroupLabel(Group *group, const char *newLabel) = 0;
    virtual void setGroupColour(Group *group, uint8_t newColour) = 0;
    virtual void setGroupVisible(Group *group, bool shouldBeVisible) = 0;
    virtual void setGroupBounds(Group *group, const Rectangle &bounds) = 0;
    virtual void setGroupSlot(Group *group,
                              uint8_t newSlot,
                              uint8_t newWidth,
                              uint8_t newHeight) = 0;

    // Patch handling
    virtual void requestAllPatches(void) = 0;

    // Component assigments
    virtual void assignComponentToControl(uint16_t controlId,
                                          Component *component) = 0;
    virtual void removeComponentFromControl(uint16_t controlId) = 0;
    virtual void assignComponentToGroup(uint8_t groupId,
                                        Component *component) = 0;
    virtual void removeComponentFromGroup(uint8_t groupId) = 0;

    // Testing
    virtual void ping(void) = 0;

    // Getters
    virtual uint8_t getCurrentPageId(void) = 0;
    virtual uint8_t getCurrentControlSetId(void) = 0;
    virtual bool isDetailLocked(void) = 0;
    virtual bool isDetailOnTheLeft(void) = 0;
};
