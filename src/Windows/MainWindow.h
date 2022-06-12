#pragma once

#include "ParameterMapWindow.h"
#include "ButtonListener.h"
#include "PageSelectionWindow.h"
#include "DetailWindow.h"
#include "PageView.h"
#include "UiDelegate.h"
#include "Model.h"
#include "Midi/Midi.h"
#include "System.h"

class MainWindow : public ParameterMapWindow, public UiDelegate
{
public:
    MainWindow(Model &newModel, Midi &newMidi);
    virtual ~MainWindow() = default;

    void onButtonDown(uint8_t buttonId) override;
    void onButtonUp(uint8_t buttonId) override;

    // Actions
    void setPage(uint8_t pageId, uint8_t controlSetId) override;
    void setControlSet(uint8_t controlSetId) override;
    void openDetail(uint16_t controlId) override;
    void lockDetail(void) override;
    void closeDetail(void) override;
    void openPageSelection(void) override;
    void closePageSelection(void) override;
    void repaintPage(void) override;
    void repaintControl(uint16_t controlId) override;

    // Lua
    void setControlVisible(uint16_t controlId, bool shouldBeVisible) override;
    void setControlName(uint16_t controlId, const char *newName) override;
    void setControlColour(uint16_t controlId, uint8_t newColour) override;

    void setControlPot(uint16_t controlId,
                       uint8_t newControlSetId,
                       uint8_t newPotId) override;
    void setControlBounds(uint16_t controlId, const Rectangle &bounds) override;
    void setControlSlot(uint16_t controlId, uint8_t newSlot) override;

    void setPageName(uint8_t pageId, const char *newName) override;

    void setGroupLabel(uint16_t groupId, const char *newLabel) override;
    void setGroupColour(uint16_t groupId, uint8_t newColour) override;
    void setGroupVisible(uint16_t groupId, bool shouldBeVisible) override;
    void setGroupBounds(uint16_t groupId, const Rectangle &bounds) override;
    void setGroupSlot(uint16_t groupId,
                      uint8_t newSlot,
                      uint8_t newWidth,
                      uint8_t newHeight) override;

    // API
    void sendSnapshotList(const char *projectId) override;
    void sendSnapshot(const char *projectId,
                      uint8_t bankNumber,
                      uint8_t slot) override;
    void sendPresetList(void) override;
    void enableMidiLearn(void) override;
    void disableMidiLearn(void) override;
    void switchPreset(uint8_t bankNumber, uint8_t slot) override;
    // updateControl(????)
    void setSnapshotSlot(const char *projectId,
                         uint8_t bankNumber,
                         uint8_t slot) override;
    void setPresetSlot(uint8_t bankNumber, uint8_t slot) override;
    void updateSnapshot(const char *projectId,
                        uint8_t bankNumber,
                        uint8_t slot,
                        const char *name,
                        Colour colour) override;
    void removeSnapshot(const char *projectId,
                        uint8_t bankNumber,
                        uint8_t slot) override;
    void swapSnapshots(const char *projectId,
                       uint8_t sourceBankNumber,
                       uint8_t sourceSlot,
                       uint8_t destBankNumber,
                       uint8_t destSlot) override;
    void setCurrentSnapshotBank(uint8_t bankNumber) override;

    // Misc
    void requestAllPatches(void) override;

    void assignComponentToControl(uint16_t controlId,
                                  Component *component) override;
    void removeComponentFromControl(uint16_t controlId) override;
    void assignComponentToGroup(uint8_t groupId, Component *component) override;
    void removeComponentFromGroup(uint8_t groupId) override;

    void ping(void) override;

    // Getters
    uint8_t getCurrentPageId(void) override;
    uint8_t getCurrentControlSetId(void) override;
    bool isDetailLocked(void) override;
    bool isDetailOnTheLeft(void) override;

private:
    void repaintParameterMap(void);
    void enableRepaintParameterMap(void);
    void disableRepaintParameterMap(void);

    Rectangle getDetailBounds(const Control &control);

    // MainWindow data
    Preset &preset;
    Snapshots &snapshots;
    Presets &presets;

    // Interface
    Midi &midi;

    // Window content
    PageView *pageView;

    // Sub-windows
    PageSelectionWindow *pageSelectionWindow;
    DetailWindow *detailWindow;

    uint8_t currentPageId;
    uint8_t currentControlSetId;
};
