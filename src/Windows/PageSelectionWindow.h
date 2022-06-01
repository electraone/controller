#pragma once

#include "Window.h"
#include "PageSelection.h"
#include "Model/Page.h"
#include "UiDelegate.h"

class PageSelectionWindow : public Window
{
private:
    PageSelectionWindow(Pages pages,
                        uint8_t newActivePage,
                        UiDelegate *newDelegate)
        : delegate(newDelegate), ps(nullptr)
    {
        PageSelection *ps =
            new PageSelection(pages, newActivePage, newDelegate);

        if (ps) {
            setOwnedContent(ps);
            setVisible(true);
        }

        setName("pageSelectionWindow");
    }

public:
    virtual ~PageSelectionWindow() = default;

    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            logMessage("callback from pageSelectionWindow");
            System::windowManager.listWindows();
        } else if (buttonId == 4) {
            logMessage("callback from pageSelectionWindow");
            buttonBroadcaster.listListeners();
        }
    }

    void onButtonUp(uint8_t buttonId)
    {
        if (buttonId == 5) {
            delegate->closePageSelection();
        }
    }

    static PageSelectionWindow *
        createPageSelectionWindow(Pages pages,
                                  uint8_t activePage,
                                  UiDelegate *newDelegate)
    {
        PageSelectionWindow *psw =
            new PageSelectionWindow(pages, activePage, newDelegate);

        if (psw) {
            psw->setName("PageSelection");
            psw->repaint();
        }

        return (psw);
    }

private:
    UiDelegate *delegate;
    PageSelection *ps;
};
