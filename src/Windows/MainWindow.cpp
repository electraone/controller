#include "MainWindow.h"
#include "ControlComponent.h"
#include "GroupControl.h"
#include "Envelope.h"
#include "luabridge.h"
#include "System.h"

MainWindow::MainWindow(Model &newModel, Midi &newMidi, Config &newConfig)
    : model(newModel),
      preset(newModel.currentPreset),
      snapshots(newModel.snapshots),
      presets(newModel.presets),
      midi(newMidi),
      setup(newConfig),
      uiApi(*this),
      pageView(nullptr),
      presetSelectionWindow(nullptr),
      pageSelectionWindow(nullptr),
      detailWindow(nullptr),
      snapshotsWindow(nullptr),
      usbHostWindow(nullptr),
      currentPageId(0),
      currentControlSetId(0),
      currentSnapshotBank(0),
      inSleepMode(false),
      controlPort(USB_MIDI_PORT_CTRL)
{
    setName("mainWindow");
    setBounds(0, 25, 1024, 575);
}

void MainWindow::resized(void)
{
}

void MainWindow::onButtonDown(uint8_t buttonId)
{
    if (getNumActivePotTouch() > 0) {
        if (buttonId == 0) {
            showDetailOfActivePotTouch();
        } else if (buttonId == 1) {
            switchToNextHandleOfActivePotTouch();
        } else if (buttonId == 2) {
            switchToPreviousHandleOfActivePotTouch();
        }
    } else {
        if (0 <= buttonId && buttonId <= 2) {
            uiApi.switchControlSet(buttonId);
        } else if (buttonId == 3) {
            requestAllPatches();
        } else if (buttonId == 4) {
            openSnapshots();
        } else if (buttonId == 5) {
            if (inSleepMode == true) {
                reboot();
            } else {
                openPageSelection();
            }
        }
    }
}

void MainWindow::onButtonUp(uint8_t buttonId)
{
}

void MainWindow::switchOff(void)
{
    for (int i = 0; i < 16000; i += 128) {
        Hardware::screen.setBacklightbrightness(i);
        delay(10);
    }
    Hardware::screen.setBacklightbrightness(65535);
    System::tasks.enableSleepMode();
    usb_stop();
    usbHost.stop();
    inSleepMode = true;
}

void MainWindow::reboot(void)
{
    Hardware::reset();
}

bool MainWindow::switchPage(uint8_t pageId)
{
    if (preset.getPage(pageId).getHasObjects()) {
        uint8_t controlSet =
            setup.uiFeatures.resetActiveControlSet
                ? preset.pages.at(pageId).getDefaultControlSetId()
                : currentControlSetId;
        return (switchPage(pageId, controlSet));
    }
    return (false);
}

bool MainWindow::switchPage(uint8_t pageId, uint8_t controlSetId)
{
    currentPageId = pageId;
    currentControlSetId = controlSetId;

    logMessage("switchPage: Page switched: page=%d, controlSetId=%d",
               currentPageId,
               currentControlSetId);

    displayPage();
    return (true);
}

bool MainWindow::switchPageNext(void)
{
    for (uint8_t i = (currentPageId + 1); i < 12; i++) {
        if (preset.getPage(i).getHasObjects()) {
            switchPage(i);
            return (true);
        }
    }
    return (false);
}

bool MainWindow::switchPagePrev(void)
{
    for (int8_t i = (currentPageId - 1); i > 0; i--) {
        if (preset.getPage(i).getHasObjects()) {
            switchPage(i);
            return (true);
        }
    }
    return (false);
}

bool MainWindow::switchControlSet(uint8_t controlSetId)
{
    if (0 <= controlSetId && controlSetId <= 2) {
        pageView->setControlSet(controlSetId);
        currentControlSetId = controlSetId;
        return (true);
    }
    return (false);
}

void MainWindow::openDetail(uint16_t controlId)
{
    const Control &control = preset.getControl(controlId);
    detailWindow = DetailWindow::createDetailWindow(control, *this);
    detailWindow->setBounds(getDetailBounds(control));
}

void MainWindow::lockDetail(void)
{
    if (detailWindow) {
        detailWindow->setLocked();
    }
}

void MainWindow::closeDetail(void)
{
    Window::close(detailWindow);
    detailWindow = nullptr;
}

void MainWindow::openPresetSelection(void)
{
    presetSelectionWindow = PresetSelectionWindow::createPresetSelectionWindow(
        presets, setup.presetBanks, uiApi);
}

void MainWindow::closePresetSelection(void)
{
    Window::close(presetSelectionWindow);
    presetSelectionWindow = nullptr;
}

void MainWindow::openPageSelection(void)
{
    pageSelectionWindow = PageSelectionWindow::createPageSelectionWindow(
        preset.pages, currentPageId, uiApi);
}

void MainWindow::closePageSelection(void)
{
    Window::close(pageSelectionWindow);
    pageSelectionWindow = nullptr;
}

void MainWindow::openUsbHostPorts(void)
{
    usbHostWindow = UsbHostWindow::createUsbHostWindow(uiApi);
}

void MainWindow::closeUsbHostPorts(void)
{
    Window::close(usbHostWindow);
    usbHostWindow = nullptr;
}

void MainWindow::openSnapshots(void)
{
    snapshotsWindow = SnapshotsWindow::createSnapshotsWindow(
        uiApi, preset.getProjectId(), currentSnapshotBank);
}

void MainWindow::closeSnapshots(void)
{
    Window::close(snapshotsWindow);
    snapshotsWindow = nullptr;
    repaintPage();
}

void MainWindow::repaintPage(void)
{
    repaint();
}

void MainWindow::repaintControl(uint16_t controlId)
{
    if (pageView) {
        Component *c = pageView->findChildById(controlId);

        if (ControlComponent *cc = dynamic_cast<ControlComponent *>(c)) {
            cc->syncComponentProperties();
            cc->repaint();
        }
    }
}

// 'todo Deduplicate loadPreset and switchPreset functions'
bool MainWindow::loadPreset(LocalFile &file)
{
    uint8_t attempt = 0;
    System::tasks.enableSpinner();

    do {
        if (presets.loadPreset(file)) {
            logMessage("handleCtrlFileReceived: preset loaded: name=%s",
                       preset.getName());
        }
    } while (!preset.isValid() && (attempt++ < 4));

    System::tasks.disableSpinner();

    closeAllWindows();
    switchPage(1, preset.getPage(1).getDefaultControlSetId());
    setInfoText("");

    if (!preset.isValid()) {
        logMessage("handleCtrlFileReceived: preset upload failed");
        return (false);
    }
    return (true);
}

bool MainWindow::loadLua(LocalFile &file)
{
    if (isLuaValid(System::context.getCurrentLuaFile())) {
        if (preset.isValid()) {
            presets.runPresetLuaScript();
            closeAllWindows();
            switchPage(1, preset.getPage(1).getDefaultControlSetId());
            return (true);
        }
    }
    return (false);
}

bool MainWindow::loadConfig(LocalFile &file)
{
    return (true);
}

void MainWindow::setControlVisible(uint16_t controlId, bool shouldBeVisible)
{
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        if (control.isVisible() != shouldBeVisible) {
            control.setVisible(shouldBeVisible);
            if (Component *component = control.getComponent()) {
                component->setVisible(shouldBeVisible);

                if (currentControlSetId == control.getControlSetId()) {
                    if (shouldBeVisible) {
                        component->assignPot(control.inputs[0].getPotId(),
                                             control.values[0].getNumSteps());
                    } else {
                        component->releasePot();
                    }
                }
            }
        }
    }
}

void MainWindow::setControlName(uint16_t controlId, const char *newName)
{
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        control.setName(newName);

        if (Component *component = control.getComponent()) {
            component->setName(newName);
            component->repaint();
        }
    }
}

void MainWindow::setControlColour(uint16_t controlId, uint32_t newColour)
{
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        control.setColour(newColour);
        if (Component *component = control.getComponent()) {
            component->repaint();
        }
    }
}

void MainWindow::setControlPot(uint16_t controlId,
                               uint8_t newControlSetId,
                               uint8_t newPotId)
{
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        control.setControlSetId(newControlSetId);
        control.inputs[0].setPotId(newPotId - 1); // \todo Fix this

        if (pageView) {
            pageView->reassignComponent(control);
        }
    }
}

void MainWindow::setControlBounds(uint16_t controlId, const Rectangle &bounds)
{
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        control.setBounds(bounds);
        if (Component *component = control.getComponent()) {
            component->setBounds(bounds);
            repaintPage();
        }
    }
}

void MainWindow::setControlSlot(uint16_t controlId, uint8_t newSlot)
{
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        uint8_t newPotId = (newSlot - 1) % Preset::MaxNumPots;
        uint8_t newControlSetId = (newSlot - 1) / Preset::MaxNumPots;
        Rectangle bounds = slotToBounds(newSlot);
        bounds.setX(bounds.getX());
        bounds.setY(bounds.getY());

        control.setBounds(bounds);
        control.setVisible(true);
        control.inputs[0].setPotId(newPotId);
        control.setControlSetId(newControlSetId);

        if (Component *component = control.getComponent()) {
            component->setBounds(bounds);
            component->setVisible(true);

            if (pageView) {
                pageView->reassignComponent(control);
            }
        }
    }
}

void MainWindow::setPageName(uint8_t pageId, const char *newName)
{
    Page &page = preset.getPage(pageId);
    if (page.isValid()) {
        page.setName(newName);

        if ((currentPageId == page.getId()) && pageView) {
            pageView->changePageName(newName);
        }
    }
}

void MainWindow::setInfoText(const char *newText)
{
    model.info.setText(newText);
    if (pageView) {
        pageView->setInfoText(newText);
    }
}

void MainWindow::setRamPercentage(uint8_t newPercentage)
{
    pageView->setRamPercentage(newPercentage);
}

void MainWindow::setGroupLabel(uint16_t groupId, const char *newLabel)
{
    Group &group = preset.getGroup(groupId);
    if (group.isValid()) {
        group.setLabel(newLabel);

        if (strlen(newLabel) == 0) {
            group.setVisible(false);
            if (Component *component = group.getComponent()) {
                component->setVisible(false);
                repaintPage();
            }
        } else {
            if (Component *component = group.getComponent()) {
                if (GroupControl *gc =
                        dynamic_cast<GroupControl *>(component)) {
                    gc->repaint();
                }
            }
        }
    }
}

void MainWindow::setGroupColour(uint16_t groupId, uint32_t newColour)
{
    Group &group = preset.getGroup(groupId);
    if (group.isValid()) {
        group.setColour(newColour);
        if (Component *component = group.getComponent()) {
            component->repaint();
        }
    }
}

void MainWindow::setGroupVisible(uint16_t groupId, bool shouldBeVisible)
{
    Group &group = preset.getGroup(groupId);
    if (group.isValid()) {
        if (group.isVisible() != shouldBeVisible) {
            group.setVisible(shouldBeVisible);
            if (Component *component = group.getComponent()) {
                component->setVisible(shouldBeVisible);
                repaintPage();
            }
        }
    }
}

void MainWindow::setGroupBounds(uint16_t groupId, const Rectangle &bounds)
{
    Group &group = preset.getGroup(groupId);
    if (group.isValid()) {
        group.setBounds(bounds);
        if (Component *component = group.getComponent()) {
            component->setBounds(bounds);
            repaintPage();
        }
    }
}

void MainWindow::setGroupSlot(uint16_t groupId,
                              uint8_t newSlot,
                              uint8_t newWidth,
                              uint8_t newHeight)
{
    Group &group = preset.getGroup(groupId);
    if (group.isValid()) {
        Rectangle bounds = slotToBounds(newSlot);

        // Keep width within the boundaries
        int rowPosition = (newSlot - 1) % 6;

        if ((rowPosition + newWidth) > 7) {
            newWidth -= rowPosition;
        }

        // Keep height within the boundaries
        int colPosition = (newSlot - 1) / 6;

        if ((colPosition + newHeight) > 6) {
            newHeight -= colPosition;
        }

        bounds.setX(bounds.getX() - 8);
        bounds.setY(bounds.getY() - 24);

        if (newHeight > 0) {
            bounds.setHeight(newHeight * 80 + ((newHeight - 1) * 8) + 2);
        } else {
            bounds.setHeight(16);
        }

        bounds.setWidth(bounds.getWidth() + 16);
        group.setBounds(bounds);

        if (Component *component = group.getComponent()) {
            component->setBounds(bounds);
            repaintPage();
        }
    }
}

void MainWindow::sendSnapshotList(uint8_t port, const char *projectId)
{
    logMessage("sendSnapshotList: projectId=%s", projectId);
    snapshots.sendList(port, projectId);
}

void MainWindow::sendSnapshot(uint8_t port,
                              const char *projectId,
                              uint8_t bankNumber,
                              uint8_t slot)
{
    logMessage("sendSnapshot: projectId=%s, bankNumber=%d, slot=%d",
               projectId,
               bankNumber,
               slot);
    snapshots.sendSnapshot(port, projectId, bankNumber, slot);
}

void MainWindow::sendPresetList(uint8_t port)
{
    logMessage("sendPresetList");
    presets.sendList(port);
}

void MainWindow::enableMidiLearn(void)
{
    logMessage("enableMidiLearn");
    System::context.setMidiLearn(true);
}

void MainWindow::disableMidiLearn(void)
{
    logMessage("disableMidiLearn");
    System::context.setMidiLearn(false);
}

void MainWindow::switchPreset(uint8_t bankNumber, uint8_t slot)
{
    logMessage("switchPreset: bankNumber=%d, slot=%d, id=%d",
               bankNumber,
               slot,
               bankNumber * Preset::MaxNumPots + slot);
    presets.loadPresetById(bankNumber * Preset::MaxNumPots + slot);
    snapshots.initialise(preset.getProjectId());
    parameterMap.setProjectId(preset.getProjectId());
    switchPage(1, preset.getPage(1).getDefaultControlSetId());
    setInfoText("");
}

void MainWindow::switchPresetNext(void)
{
    auto presetId = presets.getPresetId();

    if (presetId < 71) {
        presetId++;
        switchPreset(presetId / 12, presetId % 12);
    }
}

void MainWindow::switchPresetPrev(void)
{
    auto presetId = presets.getPresetId();

    if (presetId > 0) {
        presetId--;
        switchPreset(presetId / 12, presetId % 12);
    }
}

void MainWindow::switchPresetBank(uint8_t bankNumber)
{
    logMessage("switchPresetBank: bankNumber=%d", bankNumber);
    presets.setCurrentBankNumber(bankNumber);
}

void MainWindow::setSnapshotSlot(const char *projectId,
                                 uint8_t bankNumber,
                                 uint8_t slot)
{
    logMessage("setSnapshotSlot: projectId=%s, bankNumber=%d, slot=%d",
               projectId,
               bankNumber,
               slot);
    snapshots.setDestProjectId(projectId);
    snapshots.setDestBankNumber(bankNumber);
    snapshots.setDestSlot(slot);
}

void MainWindow::setPresetSlot(uint8_t bankNumber, uint8_t slot)
{
    logMessage("setPresetSlot: bankNumber=%d, slot=%d", bankNumber, slot);
    presets.setCurrentBankNumber(bankNumber);
    presets.setCurrentSlot(slot);
}

void MainWindow::loadSnapshot(const char *projectId,
                              uint8_t bankNumber,
                              uint8_t slot)
{
    logMessage("loading snapshot: projectId=%s, bankNumber=%d, slot=%d",
               projectId,
               bankNumber,
               slot);
    snapshots.sendSnapshotMessages(projectId, bankNumber, slot);
    sendAllControls();
}

void MainWindow::saveSnapshot(const char *projectId,
                              uint8_t bankNumber,
                              uint8_t slot,
                              const char *newName,
                              uint32_t newColour)
{
    logMessage(
        "saving snapshot: projectId=%s, bankNumber=%d, slot=%d, name=%s, colour=%d",
        projectId,
        bankNumber,
        slot,
        newName,
        newColour);
    snapshots.saveSnapshot(projectId, bankNumber, slot, newName, newColour);

    if (snapshotsWindow && strcmp(projectId, preset.getProjectId()) == 0) {
        snapshotsWindow->snapshotSaved(bankNumber, slot, newName, newColour);
    }
}

void MainWindow::updateSnapshot(const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot,
                                const char *name,
                                uint32_t colour)
{
    logMessage(
        "updateSnapshot: projectId=%s, bankNumber=%d, slot=%d, name=%s, colour=%d",
        projectId,
        bankNumber,
        slot,
        name,
        colour);
    snapshots.updateSnapshot(projectId, bankNumber, slot, name, colour);
    if (snapshotsWindow && strcmp(projectId, preset.getProjectId()) == 0) {
        snapshotsWindow->snapshotSaved(bankNumber, slot, name, colour);
    }
}

void MainWindow::removeSnapshot(const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot)
{
    logMessage("removeSnapshot: projectId=%s, bankNumber=%d, slot=%d",
               projectId,
               bankNumber,
               slot);
    snapshots.removeSnapshot(projectId, bankNumber, slot);
    if (snapshotsWindow && strcmp(projectId, preset.getProjectId()) == 0) {
        snapshotsWindow->snapshotRemoved(bankNumber, slot);
    }
}

void MainWindow::swapSnapshots(const char *projectId,
                               uint8_t sourceBankNumber,
                               uint8_t sourceSlot,
                               uint8_t destBankNumber,
                               uint8_t destSlot)
{
    logMessage(
        "swapSnapshots: projectId=%s, sourceBankNumber=%d, sourceSlot=%d, destBankNumber=%d, destSlot=%d",
        projectId,
        sourceBankNumber,
        sourceSlot,
        destBankNumber,
        destSlot);
    snapshots.swapSnapshot(
        projectId, sourceBankNumber, sourceSlot, destBankNumber, destSlot);
    if (snapshotsWindow && strcmp(projectId, preset.getProjectId()) == 0) {
        snapshotsWindow->snapshotsSwapped(
            sourceBankNumber, sourceSlot, destBankNumber, destSlot);
    }
}

bool MainWindow::importSnapshot(LocalFile &file)
{
    logMessage("importSnapshot: file: %s", file.getFilepath());
    auto snapshot = snapshots.importSnapshot(file);
    if (snapshotsWindow
        && strcmp(snapshots.getDestProjectId(), preset.getProjectId()) == 0) {
        snapshotsWindow->snapshotSaved(snapshots.getDestBankNumber(),
                                       snapshots.getDestSlot(),
                                       snapshot.getName(),
                                       snapshot.getColour());
    }
    return (true); // \todo it should not be always true
}

void MainWindow::setCurrentSnapshotBank(uint8_t bankNumber)
{
    logMessage("setCurrentSnapshotBank: bankNumber=%d", bankNumber);
    currentSnapshotBank = bankNumber;
    if (snapshotsWindow) {
        snapshotsWindow->snapshotBankSwitched(currentSnapshotBank);
    }
}

void MainWindow::setControlPort(uint8_t newControlPort)
{
    controlPort = newControlPort;
}

uint8_t MainWindow::getControlPort(void)
{
    return (controlPort);
}

void MainWindow::requestAllPatches(void)
{
    midi.requestAllPatches();
}

void MainWindow::assignComponentToControl(uint16_t controlId,
                                          Component *component)
{
#ifdef DEBUG
    if (component) {
        logMessage("assignComponentToControl: control=%d, component=%s",
                   controlId,
                   component->getName());
    }
#endif
    preset.getControl(controlId).setComponent(component);
}

void MainWindow::removeComponentFromControl(uint16_t controlId)
{
    assignComponentToControl(controlId, nullptr);
}

void MainWindow::assignComponentToGroup(uint16_t groupId, Component *component)
{
#ifdef DEBUG
    if (component) {
        logMessage("assignComponentToGroup: group=%d, component=%s",
                   groupId,
                   component->getName());
    }
#endif
    preset.getGroup(groupId).setComponent(component);
}

void MainWindow::removeComponentFromGroup(uint16_t groupId)
{
    assignComponentToGroup(groupId, nullptr);
}

void MainWindow::setDefaultValue(uint16_t controlId, uint8_t handle)
{
    Control &control = preset.getControl(controlId);
    ControlValue &value = control.getValue(handle);
    control.setDefaultValue(value, true);
}

void MainWindow::setActiveControlSetType(
    ActiveControlSetType newActiveControlSetType)
{
    setup.uiFeatures.activeControlSetType = newActiveControlSetType;
}

void MainWindow::setActivePotTouch(uint8_t potId, Component *component)
{
    Window::setActivePotTouch(potId, component);
}

void MainWindow::resetActivePotTouch(uint8_t potId)
{
    Window::resetActivePotTouch(potId);
}

void MainWindow::ping(void)
{
    logMessage("delegate ping");
}

uint8_t MainWindow::getCurrentPageId(void) const
{
    return (currentPageId);
}

uint8_t MainWindow::getCurrentControlSetId(void) const
{
    return (currentControlSetId);
}

uint8_t MainWindow::getCurrentPresetBank(void) const
{
    return (presets.getCurrentBankNumber());
}

uint8_t MainWindow::getCurrentSnapshotBank(void) const
{
    return (currentSnapshotBank);
}

const char *MainWindow::getCurrentProjectId(void) const
{
    return (preset.getProjectId());
}

bool MainWindow::isDetailLocked(void) const
{
    if (detailWindow) {
        return detailWindow->isLocked();
    }
    return false;
}

bool MainWindow::isDetailOnTheLeft(void) const
{
    if (detailWindow) {
        return (detailWindow->getX() < 512);
    }
    return false;
}

void MainWindow::displayPage(void)
{
    if (pageView) {
        delete pageView;
    }

    pageView = new PageView(preset,
                            *this,
                            uiApi,
                            setup.uiFeatures,
                            currentPageId,
                            currentControlSetId);
    replaceOwnedContent(pageView);
    setRamPercentage(getUsedMemory());
    setInfoText(model.info.getText());
}

void MainWindow::showDetailOfActivePotTouch(void)
{
    if (Component *c = getActivePotComponent()) {
        openDetail(c->getId());
    }
}

void MainWindow::switchToNextHandleOfActivePotTouch(void)
{
    for (uint8_t i = 0; i < Preset::MaxNumPots; i++) {
        if (potTouchComponents[i]) {
            switchToNextHandle(potTouchComponents[i]);
        }
    }
}

void MainWindow::switchToPreviousHandleOfActivePotTouch(void)
{
    for (uint8_t i = 0; i < Preset::MaxNumPots; i++) {
        if (potTouchComponents[i]) {
            switchToPreviousHandle(potTouchComponents[i]);
        }
    }
}

void MainWindow::showActiveHandle(Component *component, bool shouldBeShown)
{
    if (Envelope *en = dynamic_cast<Envelope *>(component)) {
        en->showActiveSegment(shouldBeShown);
    }
}

void MainWindow::switchToNextHandle(Component *component)
{
    if (Envelope *en = dynamic_cast<Envelope *>(component)) {
        en->switchToNextActiveHandle();
    }
}

void MainWindow::switchToPreviousHandle(Component *component)
{
    if (Envelope *en = dynamic_cast<Envelope *>(component)) {
        en->switchToPreviousActiveHandle();
    }
}

Rectangle MainWindow::getDetailBounds(const Control &control)
{
    if (control.getType() == Control::Type::Fader) {
        return Rectangle(8, 217, 1008, 330);
    } else if (control.getType() == Control::Type::List) {
        uint16_t xPosition = (control.getBounds().getX() > 510) ? 8 : 577;
        return Rectangle(xPosition, 25, 436, 550);
    } else if (control.getType() == Control::Type::Dx7envelope) {
        return Rectangle(158, 62, 828, 490);
    }
    return Rectangle(158, 110, 828, 380);
}

/*
void MainWindow::closeWindow(Window *window)
{
    if (window) {
        delete window;
    }
}
*/

void MainWindow::closeAllWindows(void)
{
    closeDetail();
    closePresetSelection();
    closePageSelection();
    closeUsbHostPorts();
    closeSnapshots();
}

void MainWindow::sendAllControls(void)
{
    for (auto &[id, control] : preset.controls) {
        for (auto &value : control.values) {
            value.message.setValue(
                parameterMap.getValue(value.message.getDeviceId(),
                                      value.message.getType(),
                                      value.message.getParameterNumber()));
            midi.sendMessage(value.message);
        }
    }
}

void MainWindow::initialiseEmpty(void)
{
    switchPage(1, 0);
    setInfoText("");
    presets.assignPresetNames(0);
}
