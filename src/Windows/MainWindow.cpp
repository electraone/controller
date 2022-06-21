#include "MainWindow.h"
#include "ControlComponent.h"
#include "GroupControl.h"
#include "ADSRControl.h"

MainWindow::MainWindow(Model &newModel, Midi &newMidi)
    : preset(newModel.currentPreset),
      snapshots(newModel.snapshots),
      presets(newModel.presets),
      midi(newMidi),
      pageView(nullptr),
      pageSelectionWindow(nullptr),
      detailWindow(nullptr),
      currentPageId(0),
      currentControlSetId(0),
      numActivePotTouch(0),
      potTouchComponents{ nullptr }
{
    setName("mainWindow");
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
            setControlSet(buttonId);
        } else if (buttonId == 3) {
            System::windowManager.listWindows();
            requestAllPatches();
        } else if (buttonId == 4) {
            buttonBroadcaster.listListeners();
        } else if (buttonId == 5) {
            parameterMap.listWindows();
            openPageSelection();
        }
    }
}

void MainWindow::onButtonUp(uint8_t buttonId)
{
}

void MainWindow::setPage(uint8_t pageId, uint8_t controlSetId)
{
    setVisible(false);

    delete pageView;

    PageView *newPageView = new PageView(preset, this, pageId, controlSetId);

    if (newPageView) {
        setOwnedContent(newPageView);
        currentPageId = pageId;
        currentControlSetId = controlSetId;
        pageView = newPageView;
    }

    setVisible(true);

    logMessage("Page switched: page=%d, controlSetId=%d",
               currentPageId,
               currentControlSetId);

    display();
}

void MainWindow::setControlSet(uint8_t controlSetId)
{
    setPage(currentPageId, controlSetId);
}

void MainWindow::openDetail(uint16_t controlId)
{
    const Control &control = preset.getControl(controlId);
    detailWindow = DetailWindow::createDetailWindow(control, this);
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
    closeWindow(detailWindow);
}

void MainWindow::openPageSelection(void)
{
    pageSelectionWindow = PageSelectionWindow::createPageSelectionWindow(
        preset.pages, currentPageId, this);
}

void MainWindow::closePageSelection(void)
{
    closeWindow(pageSelectionWindow);
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
            cc->onControlUpdated();
            cc->repaint();
        }
    }
}

void MainWindow::setControlVisible(uint16_t controlId, bool shouldBeVisible)
{
    logMessage("controlId=%d, visible=%d", controlId, shouldBeVisible);
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        if (control.isVisible() != shouldBeVisible) {
            control.setVisible(shouldBeVisible);
            if (Component *component = control.getComponent()) {
                component->setVisible(shouldBeVisible);
                repaintPage();
            }
        }
    }
}

void MainWindow::setControlName(uint16_t controlId, const char *newName)
{
    logMessage("controlId=%d, name=%s", controlId, newName);
    Control &control = preset.getControl(controlId);
    if (control.isValid()) {
        control.setName(newName);

        if (Component *component = control.getComponent()) {
            component->setName(newName);
            component->repaint();
        }
    }
}

void MainWindow::setControlColour(uint16_t controlId, uint8_t newColour)
{
    logMessage("controlId=%d, colour=%d", controlId, newColour);
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

void MainWindow::setGroupLabel(uint16_t groupId, const char *newLabel)
{
    Group &group = preset.getGroup(groupId);
    if (group.isValid()) {
        group.setLabel(newLabel);

        if (Component *component = group.getComponent()) {
            if (GroupControl *gc = dynamic_cast<GroupControl *>(component)) {
                gc->setLabel(newLabel);
                gc->repaint();
            }
        }
    }
}

void MainWindow::setGroupColour(uint16_t groupId, uint8_t newColour)
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
    //snapshots.sendList(port);
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
    setPage(1, preset.getPage(1).getDefaultControlSetId());
    sendPresetSwitch(2, bankNumber, slot);
}

void MainWindow::setSnapshotSlot(const char *projectId,
                                 uint8_t bankNumber,
                                 uint8_t slot)
{
    logMessage("setSnapshotSlot: projectId=%s, bankNumber=%d, slot=%d",
               projectId,
               bankNumber,
               slot);
}

void MainWindow::setPresetSlot(uint8_t bankNumber, uint8_t slot)
{
    logMessage("setPresetSlot: bankNumber=%d, slot=%d", bankNumber, slot);
    presets.setCurrentBankNumber(bankNumber);
    presets.setCurrentSlot(slot);
}

void MainWindow::updateSnapshot(const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot,
                                const char *name,
                                Colour colour)
{
    logMessage(
        "updateSnapshot: projectId=%s, bankNumber=%d, slot=%d, name=%s, colour=%d",
        projectId,
        bankNumber,
        slot,
        name,
        colour);
}

void MainWindow::removeSnapshot(const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot)
{
    logMessage("removeSnapshot: projectId=%s, bankNumber=%d, slot=%d",
               projectId,
               bankNumber,
               slot);
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
}

void MainWindow::setCurrentSnapshotBank(uint8_t bankNumber)
{
    logMessage("setCurrentSnapshotBank: bankNumber=%d", bankNumber);
}

void MainWindow::requestAllPatches(void)
{
    midi.requestAllPatches();
}

void MainWindow::assignComponentToControl(uint16_t controlId,
                                          Component *component)
{
    logMessage("assignComponentToControl: control=%d, component=%s",
               controlId,
               component->getName());
    preset.getControl(controlId).setComponent(component);
}

void MainWindow::removeComponentFromControl(uint16_t controlId)
{
    assignComponentToControl(controlId, nullptr);
}

void MainWindow::assignComponentToGroup(uint8_t groupId, Component *component)
{
    logMessage("assignComponentToGroup: group=%d, component=%s",
               groupId,
               component->getName());
    preset.getGroup(groupId).setComponent(component);
}

void MainWindow::removeComponentFromGroup(uint8_t groupId)
{
    assignComponentToGroup(groupId, nullptr);
}

void MainWindow::setActivePotTouch(uint8_t potId, Component *component)
{
    potTouchComponents[potId] = component;
    numActivePotTouch++;
}

void MainWindow::resetActivePotTouch(uint8_t potId)
{
    potTouchComponents[potId] = nullptr;
    numActivePotTouch--;
}

void MainWindow::setDefaultValue(uint16_t controlId, uint8_t handle)
{
    Control &control = preset.getControl(controlId);
    ControlValue &value = control.getValue(handle);
    control.setDefaultValue(value);
}

void MainWindow::ping(void)
{
    logMessage("delegate ping");
}

uint8_t MainWindow::getCurrentPageId(void)
{
    return (currentPageId);
}

uint8_t MainWindow::getCurrentControlSetId(void)
{
    return (currentControlSetId);
}

bool MainWindow::isDetailLocked(void)
{
    if (detailWindow) {
        return detailWindow->isLocked();
    }
    return false;
}

bool MainWindow::isDetailOnTheLeft(void)
{
    if (detailWindow) {
        return (detailWindow->getX() < 512);
    }
    return false;
}

Component *MainWindow::getActivePotComponent(void) const
{
    for (uint8_t i = 0; i < Preset::MaxNumPots; i++) {
        if (potTouchComponents[i]) {
            return (potTouchComponents[i]);
        }
    }
    return (nullptr);
}

void MainWindow::resetAllActivePotComponents(void)
{
    for (uint8_t i = 0; i < Preset::MaxNumPots; i++) {
        potTouchComponents[i] = nullptr;
    }
    numActivePotTouch = 0;
}

void MainWindow::showDetailOfActivePotTouch(void)
{
    for (uint8_t i = 0; i < Preset::MaxNumPots; i++) {
        if (potTouchComponents[i]) {
            showActiveHandle(potTouchComponents[i], false);
        }
    }
    if (Component *c = getActivePotComponent()) {
        logMessage("need detail of %d", c->getId());
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

uint8_t MainWindow::getNumActivePotTouch(void)
{
    return (numActivePotTouch);
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
    if (control.getType() == ControlType::fader) {
        return Rectangle(8, 217, 1008, 330);
    } else if (control.getType() == ControlType::list) {
        uint16_t xPosition = (control.getBounds().getX() > 510) ? 8 : 577;
        return Rectangle(xPosition, 22, 436, 556);
    } else if (control.getType() == ControlType::dx7envelope) {
        return Rectangle(158, 62, 828, 490);
    }
    return Rectangle(158, 110, 828, 380);
}

void MainWindow::closeWindow(Window *window)
{
    if (window) {
        delete window;
        window = nullptr;
        buttonBroadcaster.resumeListener(this);
        resetActiveTouch();
        resetAllActivePotComponents();
        repaint();
    }
}
