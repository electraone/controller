#pragma once

#include "Window.h"
#include "ButtonListener.h"
#include "PageSelectionWindow.h"
#include "UiDelegate.h"
#include "PageView.h"

class MainWindow : public Window, public UiDelegate, public ButtonListener
{
public:
    explicit MainWindow(const Preset &newPreset)
        : preset(newPreset),
          pageView(nullptr),
          pageSelectionWindow(nullptr),
          currentPageId(0)
    {
        setName("Main");
        assignAllButtons();
    }

    void onButtonDown(uint8_t buttonId) override;
    void onButtonUp(uint8_t buttonId) override;

    void setPage(uint8_t pageId);

private:
    const Preset &preset;
    PageView *pageView;
    PageSelectionWindow *pageSelectionWindow;
    uint8_t currentPageId;
};
