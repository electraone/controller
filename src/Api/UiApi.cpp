#include "UiApi.h"
#include "Midi/Sysex.h"

UiApi::UiApi(MainDelegate &newDelegate) : delegate(newDelegate)
{
}

void UiApi::switchPreset(uint8_t bankNumber, uint8_t slot)
{
    delegate.switchPreset(bankNumber, slot);
    delegate.closePresetSelection();
    sendPresetSwitched(USB_MIDI_PORT_CTRL, bankNumber, slot);

    // These are a temporary solution to make sure @jhh's Ableton remote script
    // can catch the preset changes done by the user on the controller.
    sendPresetSwitched(0, bankNumber, slot);
    sendPresetSwitched(1, bankNumber, slot);
}

void UiApi::switchPresetBank(uint8_t bankNumber)
{
    delegate.switchPresetBank(bankNumber);
    // \todo sendPresetBankSwitched()
}

uint8_t UiApi::getCurrentPresetBank(void)
{
    return (delegate.getCurrentPresetBank());
}

void UiApi::switchPage(uint8_t pageId)
{
    delegate.switchPage(pageId);
    // \todo sendPageSwicthed()
}

void UiApi::switchSnapshotBank(uint8_t bankNumber)
{
    delegate.setCurrentSnapshotBank(bankNumber);
    sendSnapshotBankChanged(USB_MIDI_PORT_CTRL, bankNumber);
}

void UiApi::sendAllSnapshotValues(void)
{
    delegate.sendAllControls();
}

void UiApi::loadSnapshot(const char *projectId,
                         uint8_t bankNumber,
                         uint8_t slot)
{
    delegate.loadSnapshot(projectId, bankNumber, slot);
    delegate.closeSnapshots();
}

void UiApi::loadAndStaySnapshot(const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot)
{
    delegate.loadSnapshot(projectId, bankNumber, slot);
}

void UiApi::saveSnapshot(const char *projectId,
                         uint8_t bankNumber,
                         uint8_t slot,
                         const char *name,
                         uint8_t colour)
{
    delegate.saveSnapshot(projectId, bankNumber, slot, name, colour);
    sendSnapshotChanged(USB_MIDI_PORT_CTRL);
}

void UiApi::removeSnapshot(const char *projectId,
                           uint8_t bankNumber,
                           uint8_t slot)
{
    delegate.removeSnapshot(projectId, bankNumber, slot);
    sendSnapshotChanged(USB_MIDI_PORT_CTRL);
}

void UiApi::presetSelection_openUsbHostPorts(void)
{
    delegate.closePresetSelection();
    delegate.openUsbHostPorts();
}

void UiApi::presetSelection_close(void)
{
    delegate.closePresetSelection();
}

void UiApi::presetSelection_openPageSelection(void)
{
    delegate.closePresetSelection();
    delegate.openPageSelection();
}

void UiApi::pageSelection_openUsbHostPorts(void)
{
    delegate.closePageSelection();
    delegate.openUsbHostPorts();
}

void UiApi::pageSelection_openPresetSelection(void)
{
    delegate.closePageSelection();
    delegate.openPresetSelection();
}

void UiApi::pageSelection_close(void)
{
    delegate.closePageSelection();
}

void UiApi::snapshots_close(void)
{
    delegate.closeSnapshots();
}

void UiApi::usbHost_close(void)
{
    delegate.closeUsbHostPorts();
}

void UiApi::usbHost_openPresetSelection(void)
{
    delegate.closeUsbHostPorts();
    delegate.openPresetSelection();
}

void UiApi::usbHost_openPageSelection(void)
{
    delegate.closeUsbHostPorts();
    delegate.openPageSelection();
}
