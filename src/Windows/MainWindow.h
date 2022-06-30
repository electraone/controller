#pragma once

#include "ParameterMapWindow.h"
#include "ButtonListener.h"
#include "PresetSelectionWindow.h"
#include "PageSelectionWindow.h"
#include "DetailWindow.h"
#include "SnapshotsWindow.h"
#include "UsbHostWindow.h"
#include "PageView.h"
#include "UiDelegate.h"
#include "Model.h"
#include "Midi/Midi.h"
#include "Setup/Setup.h"
#include "System.h"

class MainWindow : public ParameterMapWindow, public UiDelegate
{
public:
    MainWindow(Model &newModel, Midi &newMidi, Setup &newSetup);
    virtual ~MainWindow() = default;

    void onButtonDown(uint8_t buttonId) override;
    void onButtonUp(uint8_t buttonId) override;

    // Actions
    void switchPage(uint8_t pageId) override;
    void switchPage(uint8_t pageId, uint8_t controlSetId) override;
    void switchPageNext(void) override;
    void switchPagePrev(void) override;
    void setControlSet(uint8_t controlSetId) override;
    void openDetail(uint16_t controlId) override;
    void lockDetail(void) override;
    void closeDetail(void) override;
    void openPresetSelection(void) override;
    void closePresetSelection(void) override;
    void openPageSelection(void) override;
    void closePageSelection(void) override;
    void openUsbHostPorts(void) override;
    void closeUsbHostPorts(void) override;
    void openSnapshots(void) override;
    void closeSnapshots(void) override;
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
    void sendSnapshotList(uint8_t port, const char *projectId) override;
    void sendSnapshot(uint8_t port,
                      const char *projectId,
                      uint8_t bankNumber,
                      uint8_t slot) override;
    void sendPresetList(uint8_t port) override;
    void enableMidiLearn(void) override;
    void disableMidiLearn(void) override;
    void switchPreset(uint8_t bankNumber, uint8_t slot) override;
    void switchPresetNext(void) override;
    void switchPresetPrev(void) override;
    void switchPresetBank(uint8_t bankNumber) override;
    void setSnapshotSlot(const char *projectId,
                         uint8_t bankNumber,
                         uint8_t slot) override;
    void setPresetSlot(uint8_t bankNumber, uint8_t slot) override;
    void loadSnapshot(const char *projectId,
                      uint8_t bankNumber,
                      uint8_t slot) override;
    void saveSnapshot(const char *projectId,
                      uint8_t bankNumber,
                      uint8_t slot,
                      const char *newName,
                      uint8_t newColour) override;
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

    void sendAllControls(void);

    // Misc
    void requestAllPatches(void) override;

    void assignComponentToControl(uint16_t controlId,
                                  Component *component) override;
    void removeComponentFromControl(uint16_t controlId) override;
    void assignComponentToGroup(uint8_t groupId, Component *component) override;
    void removeComponentFromGroup(uint8_t groupId) override;

    void setActivePotTouch(uint8_t potId, Component *component) override;
    void resetActivePotTouch(uint8_t potId) override;

    void setDefaultValue(uint16_t controlId, uint8_t handle) override;

    void setActiveControlSetType(
        ActiveControlSetType newActiveControlSetType) override;
    void closeAllWindows(void) override;

    void ping(void) override;

    // Getters
    uint8_t getCurrentPageId(void) override;
    uint8_t getCurrentControlSetId(void) override;
    uint8_t getCurrentPresetBank(void) override;
    bool isDetailLocked(void) override;
    bool isDetailOnTheLeft(void) override;

private:
    void displayPage(void);
    Component *getActivePotComponent(void) const;
    void resetAllActivePotComponents(void);
    void showDetailOfActivePotTouch(void);
    uint8_t getNumActivePotTouch(void);
    void showActiveHandle(Component *component, bool shouldBeShown);
    void switchToNextHandle(Component *component);
    void switchToPreviousHandle(Component *component);
    void switchToNextHandleOfActivePotTouch(void);
    void switchToPreviousHandleOfActivePotTouch(void);
    Rectangle getDetailBounds(const Control &control);
    void closeWindow(Window *window);

    // MainWindow data
    Preset &preset;
    Snapshots &snapshots;
    Presets &presets;

    // Interface
    Midi &midi;

    // Setup
    Setup &setup;

    // Window content
    PageView *pageView;

    // Sub-windows
    PresetSelectionWindow *presetSelectionWindow;
    PageSelectionWindow *pageSelectionWindow;
    DetailWindow *detailWindow;
    SnapshotsWindow *snapshotsWindow;
    UsbHostWindow *usbHostWindow;

    uint8_t currentPageId;
    uint8_t currentControlSetId;

    // Ui state
    uint8_t numActivePotTouch;
    Component *potTouchComponents[Preset::MaxNumPots];
};
