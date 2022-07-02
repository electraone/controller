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
                        UiDelegate &newDelegate)
        : delegate(newDelegate), content(nullptr)
    {
        content = new PageSelection(pages, newActivePage, newDelegate);

        if (content) {
            setOwnedContent(content);
        }

        setName("PageSelectionWindow");
        setVisible(true);
    }

public:
    virtual ~PageSelectionWindow() = default;

    // API

    // Events
    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            delegate.closePageSelection();
            delegate.openUsbHostPorts();
        } else if (buttonId == 4) {
            delegate.closePageSelection();
            delegate.openPresetSelection();
        }
    }

    void onButtonUp(uint8_t buttonId)
    {
        if (buttonId == 5) {
            delegate.closePageSelection();
        }
    }

    // Factory function
    static PageSelectionWindow *
        createPageSelectionWindow(Pages pages,
                                  uint8_t activePage,
                                  UiDelegate &newDelegate)
    {
        return new PageSelectionWindow(pages, activePage, newDelegate);
    }

private:
    UiDelegate &delegate;
    PageSelection *content;
};
