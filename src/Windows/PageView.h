#pragma once

#include "ControlComponent.h"
#include "GroupControl.h"
#include "BottomBar.h"
#include "Preset.h"
#include "MainDelegate.h"
#include "UiApi.h"
#include "UiFeatures.h"

class PageView : public Component
{
public:
    PageView(const Preset &preset,
             MainDelegate &newDelegate,
             UiApi &uiApi,
             const UiFeatures &newUiFeatures,
             uint8_t newPageId,
             uint8_t activeControlSetId = 0);
    virtual ~PageView();

    void setControlSet(uint8_t newControlSetId);
    void onTouchDown(const TouchEvent &touchEvent) override;
    void reassignComponent(const Control &control);
    void changePageName(const char *newName);
    void setInfoText(const char *newText);
    void setRamPercentage(uint8_t newPercentage);

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    void addControls(const Controls &controls);
    void addGroups(const Groups &groups);
    void addBottomBar(const char *presetName, const char *pageName);
    void configureGroup(GroupControl *g, const Group &group);
    void configureControl(ControlComponent *cc, const Control &control);

    const Preset &model;
    MainDelegate &delegate;
    UiApi &uiApi;
    const UiFeatures &uiFeatures;
    uint8_t pageId;
    uint8_t controlSetId;
    BottomBar *bottomBar;
};
