#pragma once

#include "Window.h"
#include "PageSelection.h"
#include "Model/Page.h"
#include "UiApi.h"

class PageSelectionWindow : public Window
{
private:
    PageSelectionWindow(Pages pages, uint8_t activePage, UiApi &newUiApi)
        : uiApi(newUiApi), content(nullptr)
    {
        content = new PageSelection(pages, activePage, newUiApi);

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
            uiApi.pageSelection_openUsbHostPorts();
        } else if (buttonId == 4) {
            uiApi.pageSelection_openPresetSelection();
        }
    }

    void onButtonUp(uint8_t buttonId)
    {
        if (buttonId == 5) {
            uiApi.pageSelection_close();
        }
    }

    // Factory function
    static PageSelectionWindow *createPageSelectionWindow(Pages pages,
                                                          uint8_t activePage,
                                                          UiApi &newUiApi)
    {
        return new PageSelectionWindow(pages, activePage, newUiApi);
    }

private:
    UiApi &uiApi;
    PageSelection *content;
};
