#include "MainWindow.h"
#include "ControlComponent.h"
#include "GroupControl.h"

MainWindow::MainWindow(Preset &newPreset)
    : preset(newPreset),
      pageView(nullptr),
      pageSelectionWindow(nullptr),
      detailWindow(nullptr),
      currentPageId(0),
      currentControlSetId(0)
{
    setName("mainWindow");
}

void MainWindow::onButtonDown(uint8_t buttonId)
{
    if (0 <= buttonId && buttonId <= 2) {
        setControlSet(buttonId);
    } else if (buttonId == 3) {
        logMessage("callback from mainWindow");
        System::windowManager.listWindows();
    } else if (buttonId == 4) {
        logMessage("callback from mainWindow");
        buttonBroadcaster.listListeners();
    } else if (buttonId == 5) {
        logMessage("callback from mainWindow");
        parameterMap.listWindows();
        openPageSelection();
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
    if (detailWindow) {
        delete detailWindow;
        detailWindow = nullptr;
        buttonBroadcaster.resumeListener(this);
        resetActiveTouch();
        repaint();
    }
}

void MainWindow::openPageSelection(void)
{
    pageSelectionWindow = PageSelectionWindow::createPageSelectionWindow(
        preset.pages, currentPageId, this);
}

void MainWindow::closePageSelection(void)
{
    if (pageSelectionWindow) {
        delete pageSelectionWindow;
        pageSelectionWindow = nullptr;
        buttonBroadcaster.resumeListener(this);
        resetActiveTouch();
        repaint();
    }
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

void MainWindow::setControlVisible(Control *control, bool shouldBeVisible)
{
    if (control->isVisible() != shouldBeVisible) {
        control->setVisible(shouldBeVisible);
        control->getComponent()->setVisible(shouldBeVisible);
        repaintPage();
    }
}

void MainWindow::setControlName(Control *control, const char *newName)
{
    control->setName(newName);
    control->getComponent()->setName(newName);
    control->getComponent()->repaint();
}

void MainWindow::setControlColour(Control *control, uint8_t newColour)
{
    control->setColour(newColour);
    control->getComponent()->repaint();
}

void MainWindow::setControlPot(Control *control,
                               uint8_t newControlSetId,
                               uint8_t newPotId)
{
    control->setControlSetId(newControlSetId);
    control->inputs[0].setPotId(newPotId - 1); // \todo Fix this

    if (pageView) {
        pageView->reassignComponent(*control);
    }
}

void MainWindow::setControlBounds(Control *control, const Rectangle &bounds)
{
    control->setBounds(bounds);
    control->getComponent()->setBounds(bounds);
    repaintPage();
}

void MainWindow::setControlSlot(Control *control, uint8_t newSlot)
{
    uint8_t newPotId = (newSlot - 1) % 12;
    uint8_t newControlSetId = (newSlot - 1) / 12;
    Rectangle bounds = slotToBounds(newSlot);
    bounds.setX(bounds.getX());
    bounds.setY(bounds.getY());

    control->setBounds(bounds);
    control->setVisible(true);
    control->inputs[0].setPotId(newPotId);
    control->setControlSetId(newControlSetId);

    auto component = control->getComponent();

    if (component) {
        component->setBounds(bounds);
        component->setVisible(true);

        if (pageView) {
            pageView->reassignComponent(*control);
        }
    }
}

void MainWindow::setPageName(Page *page, const char *newName)
{
    page->setName(newName);

    if ((currentPageId == page->getId()) && pageView) {
        pageView->changePageName(newName);
    }
}

void MainWindow::setGroupLabel(Group *group, const char *newLabel)
{
    group->setLabel(newLabel);

    if (GroupControl *gc =
            dynamic_cast<GroupControl *>(group->getComponent())) {
        gc->setLabel(newLabel);
    }
    group->getComponent()->repaint();
}

void MainWindow::setGroupColour(Group *group, uint8_t newColour)
{
    group->setColour(newColour);
    group->getComponent()->repaint();
}

void MainWindow::setGroupVisible(Group *group, bool shouldBeVisible)
{
    if (group->isVisible() != shouldBeVisible) {
        group->setVisible(shouldBeVisible);
        group->getComponent()->setVisible(shouldBeVisible);
        repaintPage();
    }
}

void MainWindow::setGroupBounds(Group *group, const Rectangle &bounds)
{
    group->setBounds(bounds);
    group->getComponent()->setBounds(bounds);
    repaintPage();
}

void MainWindow::setGroupSlot(Group *group,
                              uint8_t newSlot,
                              uint8_t newWidth,
                              uint8_t newHeight)
{

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

/*
    if (group) {
        GroupGraphics *groupGraphics = group->getGroupGraphics();

        Rectangle bounds = slotToBounds(slot);

        bounds.setX(bounds.getX() + 8);
        bounds.setWidth(width * 159 + ((width - 1) * 21));

        if (height > 0) {
            bounds.setHeight(height * 80 + ((height - 1) * 8) + 2);
        } else {
            bounds.setHeight(16);
        }

        if (groupGraphics) {
            groupGraphics->setBounds(bounds);
            repaintPageGraphics(groupGraphics);
        }
    }
    */

    Rectangle bounds = slotToBounds(newSlot);

    bounds.setX(bounds.getX() - 8);
    bounds.setY(bounds.getY() - 24);

    if (newHeight > 0) {
        bounds.setHeight(newHeight * 80 + ((newHeight - 1) * 8) + 2);
    } else {
        bounds.setHeight(16);
    }

    bounds.setWidth(bounds.getWidth() + 16);

    group->setBounds(bounds);

    auto component = group->getComponent();

    if (component) {
        component->setBounds(bounds);
        repaintPage();
    }
}

void MainWindow::requestAllPatches(void)
{
    logMessage("need to request all patches");
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

Rectangle MainWindow::getDetailBounds(const Control &control)
{
    if (control.getType() == ControlType::fader) {
        return Rectangle(8, 217, 1008, 330);
    } else if (control.getType() == ControlType::list) {
        uint16_t xPosition = (control.getBounds().getX() > 510) ? 8 : 577;
        return Rectangle(xPosition, 22, 436, 556);
    }
    return Rectangle(58, 60, 908, 480);
}
