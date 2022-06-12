#pragma once

#include "Component.h"

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
    virtual void setControlVisible(uint16_t controlId,
                                   bool shouldBeVisible) = 0;
    virtual void setControlName(uint16_t controlId, const char *newName) = 0;
    virtual void setControlColour(uint16_t controlId, uint8_t newColour) = 0;
    virtual void setControlPot(uint16_t controlId,
                               uint8_t newControlSetId,
                               uint8_t newPotId) = 0;
    virtual void setControlBounds(uint16_t controlId,
                                  const Rectangle &bounds) = 0;
    virtual void setControlSlot(uint16_t controlId, uint8_t newSlot) = 0;

    // Pages
    virtual void setPageName(uint8_t pageId, const char *newName) = 0;

    // Groups
    virtual void setGroupLabel(uint16_t groupId, const char *newLabel) = 0;
    virtual void setGroupColour(uint16_t groupId, uint8_t newColour) = 0;
    virtual void setGroupVisible(uint16_t groupId, bool shouldBeVisible) = 0;
    virtual void setGroupBounds(uint16_t groupId, const Rectangle &bounds) = 0;
    virtual void setGroupSlot(uint16_t groupId,
                              uint8_t newSlot,
                              uint8_t newWidth,
                              uint8_t newHeight) = 0;
    // Api
    virtual void sendSnapshotList(const char *projectId) = 0;
    virtual void sendSnapshot(const char *projectId,
                              uint8_t bankNumber,
                              uint8_t slot) = 0;
    virtual void sendPresetList(void) = 0;
    virtual void enableMidiLearn(void) = 0;
    virtual void disableMidiLearn(void) = 0;
    virtual void switchPreset(uint8_t bankNumber, uint8_t slot) = 0;
    virtual void setSnapshotSlot(const char *projectId,
                                 uint8_t bankNumber,
                                 uint8_t slot) = 0;
    virtual void setPresetSlot(uint8_t bankNumber, uint8_t slot) = 0;
    virtual void updateSnapshot(const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot,
                                const char *name,
                                Colour colour) = 0;
    virtual void removeSnapshot(const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot) = 0;
    virtual void swapSnapshots(const char *projectId,
                               uint8_t sourceBankNumber,
                               uint8_t sourceSlot,
                               uint8_t destBankNumber,
                               uint8_t destSlot) = 0;
    virtual void setCurrentSnapshotBank(uint8_t bankNumber) = 0;

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
