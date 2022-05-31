#include "MainWindow.h"
#include "ControlComponent.h"

MainWindow::MainWindow(const Preset &newPreset)
    : preset(newPreset),
      pageView(nullptr),
      pageSelectionWindow(nullptr),
      detailWindow(nullptr),
      currentPageId(0)
{
    setName("mainWindow");
    assignAllButtons();
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

void MainWindow::setPage(uint8_t pageId)
{
    setVisible(false);

    delete pageView;

    PageView *newPageView = new PageView(preset, pageId);

    if (newPageView) {
        setOwnedContent(newPageView);
        currentPageId = pageId;
        pageView = newPageView;
    }

    setVisible(true);

    logMessage("Page switched: page=%d", currentPageId);

    display();
}

void MainWindow::setControlSet(uint8_t controlSetId)
{
    setVisible(false);

    delete pageView;

    PageView *newPageView = new PageView(preset, currentPageId, controlSetId);

    if (newPageView) {
        setOwnedContent(newPageView);
        pageView = newPageView;
    }

    setVisible(true);

    logMessage("controlSet switched: controlSetId=%d", controlSetId);

    display();
}

void MainWindow::openDetail(uint16_t controlId)
{
    const Control &control = preset.getControl(controlId);
    detailWindow = DetailWindow::createDetailWindow(control, this);
    detailWindow->setBounds(getDetailBounds(control));
}

void MainWindow::lockDetail(void)
{
    logMessage("delegate lock window");
    if (detailWindow) {
        detailWindow->setLocked();
    }
}
void MainWindow::closeDetail(void)
{
    logMessage("delegate close window");
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
        pageSelectionWindow = nullptr;
        buttonBroadcaster.resumeListener(this);
        resetActiveTouch();
        repaint();
    }
}

void MainWindow::ping(void)
{
    logMessage("delegate ping");
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
