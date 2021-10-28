#include "MainWindow.h"

void MainWindow::onButtonDown(uint8_t buttonId)
{
    if (buttonId == 5) {
        pageSelectionWindow = PageSelectionWindow::createPageSelectionWindow(
            preset.pages, currentPageId, this);

        pageSelectionWindow->onWindowClose = [this]() {
            logMessage("page selection was closed");
        };

    }
}

//#include "Hardware.h"
void MainWindow::onButtonUp(uint8_t buttonId)
{
    if (buttonId == 5) {
        if (pageSelectionWindow) {
            delete pageSelectionWindow;
            pageSelectionWindow = nullptr;
            repaint();
        }
    }
    //logMessage("heap: %d", Hardware::ram.heap_used());
}

void MainWindow::setPage(uint8_t pageId)
{
    setVisible(false);

    PageView *pageViewToDelete = pageView;
    PageView *newPageView = new PageView(preset, pageId);

    if (newPageView) {
        setOwnedContent(newPageView);
        currentPageId = pageId;

        if (pageViewToDelete) {
            delete pageViewToDelete;
        }
    }

    setVisible(true);

    logMessage("Page switched: page=%d", currentPageId);

    repaint();
}
