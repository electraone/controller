#pragma once

#include "ParameterMapWindow.h"
#include "ButtonListener.h"
#include "PageSelectionWindow.h"
#include "DetailWindow.h"
#include "PageView.h"
#include "UiDelegate.h"
#include "Model/Preset.h"

#include "System.h"

class MainWindow : public ParameterMapWindow, public UiDelegate
{
public:
    explicit MainWindow(const Preset &newPreset);
    virtual ~MainWindow() = default;

    void onButtonDown(uint8_t buttonId) override;
    void onButtonUp(uint8_t buttonId) override;

    // Actions
    void setPage(uint8_t pageId) override;
    void setControlSet(uint8_t controlSetId) override;
    void openDetail(uint16_t controlId) override;
    void lockDetail(void) override;
    void closeDetail(void) override;
    void openPageSelection(void) override;
    void closePageSelection(void) override;
    void ping(void) override;

    // Getters
    bool isDetailLocked(void) override;
    bool isDetailOnTheLeft(void) override;

private:
    void repaintParameterMap(void);
    void enableRepaintParameterMap(void);
    void disableRepaintParameterMap(void);

    Rectangle getDetailBounds(const Control &control);

    // MainWindow data
    const Preset &preset;
    PageView *pageView;
    uint8_t currentPageId;

    // Sub-windows
    PageSelectionWindow *pageSelectionWindow;
    DetailWindow *detailWindow;
};
