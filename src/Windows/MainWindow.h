#pragma once

#include "Window.h"
#include "ButtonListener.h"
#include "PageSelectionWindow.h"
#include "DetailWindow.h"
#include "PageView.h"
#include "UiDelegate.h"
#include "Model/Preset.h"

class MainWindow : public Window, public UiDelegate, public ButtonListener
{
public:
    explicit MainWindow(const Preset &newPreset)
        : preset(newPreset),
          pageView(nullptr),
          pageSelectionWindow(nullptr),
          detailWindow(nullptr),
          currentPageId(0)
    {
        setName("mainWindow");
        assignAllButtons();
    }

    void onButtonDown(uint8_t buttonId) override;
    void onButtonUp(uint8_t buttonId) override;

    void setPage(uint8_t pageId) override;
    void setControlSet(uint8_t controlSetId) override;
    void displayDetail(uint16_t controlId) override;

private:
    const Preset &preset;
    PageView *pageView;
    PageSelectionWindow *pageSelectionWindow;
    DetailWindow *detailWindow;
    uint8_t currentPageId;
};
