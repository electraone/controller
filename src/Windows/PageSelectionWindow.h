#pragma once

#include "PopupWindow.h"
#include "PageSelection.h"
#include "PageSelectionWindowDelegate.h"
#include "Model/Page.h"
#include "UiApi.h"

class PageSelectionWindow : public PopupWindow,
                            public PageSelectionWindowDelegate
{
private:
    PageSelectionWindow(Pages pages, uint8_t activePage, UiApi &newUiApi)
        : PopupWindow(new PageSelection(*this,
                                        pages,
                                        activePage,
                                        newUiApi,
                                        colour,
                                        activeColour),
                      colour,
                      activeColour),
          uiApi(newUiApi),
          pagesBrowsed(false),
          menuButtonReleased(false)
    {
        setName("PAGES");
        setBounds(0, 333, 1024, 235);
    }

public:
    virtual ~PageSelectionWindow() = default;

    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 2) {
            if (Hardware::buttons[BUTTON_RIGHT_BOTTOM]->isPressed()) {
                uiApi.switchOff();
            }
        } else if (buttonId == 3) {
            uiApi.pageSelection_openUsbHostPorts();
        } else if (buttonId == 4) {
            uiApi.pageSelection_openPresetSelection();
        } else if (buttonId == 5) {
            uiApi.pageSelection_close();
        }
    }

    void onButtonUp(uint8_t buttonId)
    {
        if (buttonId == 5) {
            if (pagesBrowsed) {
                uiApi.pageSelection_close();
            }
            menuButtonReleased = true;
        }
    }

    void userBrowsedPages(void) override
    {
        if (menuButtonReleased) {
            uiApi.pageSelection_close();
        }
        pagesBrowsed = true;
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
    bool pagesBrowsed;
    bool menuButtonReleased;

    static constexpr uint32_t colour = 0x00091A;
    static constexpr uint32_t activeColour = 0x001130;
};
