#include "MainWindow.h"

void MainWindow::onButtonDown(uint8_t buttonId)
{
    if (0 <= buttonId && buttonId <= 2) {
        setControlSet(buttonId);
    } else if (buttonId == 5) {
        pageSelectionWindow = PageSelectionWindow::createPageSelectionWindow(
            preset.pages, currentPageId, this);

        pageSelectionWindow->onWindowClose = [this]() {
#ifdef DEBUG
            logMessage("page selection was closed");
#endif
        };
    }
}

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
        pageView = newPageView;

        if (pageViewToDelete) {
            delete pageViewToDelete;
        }
    }

    setVisible(true);

    logMessage("Page switched: page=%d", currentPageId);

    display();
}

void MainWindow::setControlSet(uint8_t controlSetId)
{
    setVisible(false);

    PageView *pageViewToDelete = pageView;
    PageView *newPageView = new PageView(preset, currentPageId, controlSetId);

    if (newPageView) {
        setOwnedContent(newPageView);
        pageView = newPageView;

        if (pageViewToDelete) {
            delete pageViewToDelete;
        }
    }

    setVisible(true);

    logMessage("controlSet switched: controlSetId=%d", controlSetId);

    display();
}
