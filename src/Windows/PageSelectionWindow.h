#pragma once

#include "Window.h"
#include "PageSelection.h"
#include "Model/Page.h"
#include "UiDelegate.h"

class PageSelectionWindow : public Window
{
public:
    PageSelectionWindow(Pages pages,
                        uint8_t newActivePage,
                        UiDelegate *newDelegate)
    {
        PageSelection *ps =
            new PageSelection(pages, newActivePage, newDelegate);

        if (ps) {
            setOwnedContent(ps);
            setVisible(true);
        }
    }

    ~PageSelectionWindow()
    {
        if (onWindowClose) {
            onWindowClose();
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

    // \todo Do we need this?
    std::function<void(void)> onWindowClose;

private:
};
