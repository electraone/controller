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
    explicit MainWindow(Preset &newPreset);
    virtual ~MainWindow() = default;

    void onButtonDown(uint8_t buttonId) override;
    void onButtonUp(uint8_t buttonId) override;

    // Actions
    void setPage(uint8_t pageId, uint8_t controlSetId) override;
    void setControlSet(uint8_t controlSetId) override;
    void openDetail(uint16_t controlId) override;
    void lockDetail(void) override;
    void closeDetail(void) override;
    void openPageSelection(void) override;
    void closePageSelection(void) override;
    void repaintPage(void) override;
    void repaintControl(uint16_t controlId) override;

    void setControlVisible(Control *control, bool shouldBeVisible) override;
    void setControlName(Control *control, const char *newName) override;
    void setControlColour(Control *control, uint8_t newColour) override;
    void setControlPot(Control *control,
                       uint8_t newControlSetId,
                       uint8_t newPotId) override;
    void setControlBounds(Control *control, const Rectangle &bounds) override;
    void setControlSlot(Control *control, uint8_t newSlot) override;

    void setPageName(Page *page, const char *newName) override;

    void setGroupLabel(Group *group, const char *newLabel) override;
    void setGroupColour(Group *group, uint8_t newColour) override;
    void setGroupVisible(Group *group, bool shouldBeVisible) override;
    void setGroupBounds(Group *group, const Rectangle &bounds) override;
    void setGroupSlot(Group *group,
                      uint8_t newSlot,
                      uint8_t newWidth,
                      uint8_t newHeight) override;

    void requestAllPatches(void) override;

    void assignComponentToControl(uint16_t controlId,
                                  Component *component) override;
    void removeComponentFromControl(uint16_t controlId) override;
    void assignComponentToGroup(uint8_t groupId, Component *component) override;
    void removeComponentFromGroup(uint8_t groupId) override;

    void ping(void) override;

    // Getters
    uint8_t getCurrentPageId(void) override;
    uint8_t getCurrentControlSetId(void) override;
    bool isDetailLocked(void) override;
    bool isDetailOnTheLeft(void) override;

private:
    void repaintParameterMap(void);
    void enableRepaintParameterMap(void);
    void disableRepaintParameterMap(void);

    Rectangle getDetailBounds(const Control &control);

    // MainWindow data
    Preset &preset;
    PageView *pageView;
    uint8_t currentPageId;
    uint8_t currentControlSetId;

    // Sub-windows
    PageSelectionWindow *pageSelectionWindow;
    DetailWindow *detailWindow;
};
