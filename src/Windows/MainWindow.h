#pragma once

#include "ParameterMapWindow.h"
#include "ButtonListener.h"
#include "PresetSelectionWindow.h"
#include "PageSelectionWindow.h"
#include "DetailWindow.h"
#include "SnapshotsWindow.h"
#include "UsbHostWindow.h"
#include "PageView.h"
#include "MainDelegate.h"
#include "Model.h"
#include "Midi/Midi.h"
#include "Config/Config.h"
#include "System.h"

class MainWindow : public ParameterMapWindow, public MainDelegate
{
public:
    MainWindow(Model &newModel, Midi &newMidi, Config &newConfig);
    virtual ~MainWindow() = default;

    void resized(void) override;

    void onButtonDown(uint8_t buttonId) override;
    void onButtonUp(uint8_t buttonId) override;

    void switchOff(void) override;
    void reboot(void) override;

    // Actions
    bool switchPage(uint8_t pageId) override;
    bool switchPage(uint8_t pageId, uint8_t controlSetId) override;
    bool switchPageNext(void) override;
    bool switchPagePrev(void) override;
    bool switchControlSet(uint8_t controlSetId) override;
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
    void displayPage(void) override;

    //
    bool loadPreset(LocalFile &file) override;
    bool loadLua(LocalFile &file) override;
    bool loadConfig(LocalFile &file) override;
    bool importSnapshot(LocalFile &file) override;

    // Lua
    void setControlVisible(uint16_t controlId, bool shouldBeVisible) override;
    void setControlName(uint16_t controlId, const char *newName) override;
    void setControlColour(uint16_t controlId, uint32_t newColour) override;

    void setControlPot(uint16_t controlId,
                       uint8_t newControlSetId,
                       uint8_t newPotId) override;
    void setControlBounds(uint16_t controlId, const Rectangle &bounds) override;
    void setControlSlot(uint16_t controlId, uint8_t newSlot) override;

    void setPageName(uint8_t pageId, const char *newName) override;
    void setInfoText(const char *newText) override;
    void setRamPercentage(uint8_t newPercentage) override;

    void setGroupLabel(uint16_t groupId, const char *newLabel) override;
    void setGroupColour(uint16_t groupId, uint32_t newColour) override;
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
                      uint32_t newColour) override;
    void updateSnapshot(const char *projectId,
                        uint8_t bankNumber,
                        uint8_t slot,
                        const char *name,
                        uint32_t colour) override;
    void removeSnapshot(const char *projectId,
                        uint8_t bankNumber,
                        uint8_t slot) override;
    void swapSnapshots(const char *projectId,
                       uint8_t sourceBankNumber,
                       uint8_t sourceSlot,
                       uint8_t destBankNumber,
                       uint8_t destSlot) override;
    void setCurrentSnapshotBank(uint8_t bankNumber) override;
    void setControlPort(uint8_t newControlPort) override;
    uint8_t getControlPort(void) override;

    void sendAllControls(void);

    // Misc
    void requestAllPatches(void) override;

    void assignComponentToControl(uint16_t controlId,
                                  Component *component) override;
    void removeComponentFromControl(uint16_t controlId) override;
    void assignComponentToGroup(uint16_t groupId, Component *component) override;
    void removeComponentFromGroup(uint16_t groupId) override;

    void setDefaultValue(uint16_t controlId, uint8_t handle) override;

    void setActiveControlSetType(
        ActiveControlSetType newActiveControlSetType) override;
    void closeAllWindows(void) override;

    void setActivePotTouch(uint8_t potId, Component *component) override;
    void resetActivePotTouch(uint8_t potId) override;

    void ping(void) override;

    // Getters
    uint8_t getCurrentPageId(void) const override;
    uint8_t getCurrentControlSetId(void) const override;
    uint8_t getCurrentPresetBank(void) const override;
    uint8_t getCurrentSnapshotBank(void) const override;
    const char *getCurrentProjectId(void) const override;
    bool isDetailLocked(void) const override;
    bool isDetailOnTheLeft(void) const override;

    void initialiseEmpty(void) override;

private:
    void showDetailOfActivePotTouch(void);
    void showActiveHandle(Component *component, bool shouldBeShown);
    void switchToNextHandle(Component *component);
    void switchToPreviousHandle(Component *component);
    void switchToNextHandleOfActivePotTouch(void);
    void switchToPreviousHandleOfActivePotTouch(void);
    Rectangle getDetailBounds(const Control &control);

    // MainWindow data
    Model &model;
    Preset &preset;
    Snapshots &snapshots;
    Presets &presets;

    // MIDI Interface
    Midi &midi;

    // Config
    Config &setup;

    // UI Activity API handler
    UiApi uiApi;

    // Window content
    PageView *pageView;

    // Sub-windows
    PageSelectionWindow *pageSelectionWindow;
    PresetSelectionWindow *presetSelectionWindow;
    DetailWindow *detailWindow;
    SnapshotsWindow *snapshotsWindow;
    UsbHostWindow *usbHostWindow;

    // Current state
    uint8_t currentPageId;
    uint8_t currentControlSetId;
    uint8_t currentSnapshotBank;

    // Controller status
    bool inSleepMode;
    uint8_t controlPort;
};
