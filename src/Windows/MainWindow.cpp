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

MainWindow::~MainWindow()
{
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
    detailWindow =
        DetailWindow::createDetailWindow(preset.getControl(controlId), this);
}

void MainWindow::closeDetail(void)
{
    if (detailWindow) {
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
