#pragma once

#include "MainDelegate.h"

class UiApi
{
public:
    explicit UiApi(MainDelegate &newDelegate);
    virtual ~UiApi() = default;

    void switchOff(void);
    void reboot(void);
    void switchPreset(uint8_t bankNumber, uint8_t slot);
    void switchPresetBank(uint8_t bankNumber);
    uint8_t getCurrentPresetBank(void);

    void switchPage(uint8_t pageId);

    void switchSnapshotBank(uint8_t newBankNumber);
    void sendAllSnapshotValues(void);
    void loadSnapshot(const char *projectId, uint8_t bankNumber, uint8_t slot);
    void loadAndStaySnapshot(const char *projectId,
                             uint8_t bankNumber,
                             uint8_t slot);
    void saveSnapshot(const char *projectId,
                      uint8_t bankNumber,
                      uint8_t slot,
                      const char *name,
                      uint16_t colour);
    void
        removeSnapshot(const char *projectId, uint8_t bankNumber, uint8_t slot);

    void presetSelection_openUsbHostPorts(void);
    void presetSelection_close(void);
    void presetSelection_openPageSelection(void);

    void pageSelection_openUsbHostPorts(void);
    void pageSelection_openPresetSelection(void);
    void pageSelection_close(void);

    void snapshots_close(void);

    void usbHost_close(void);
    void usbHost_openPresetSelection(void);
    void usbHost_openPageSelection(void);

private:
    MainDelegate &delegate;
};
