#pragma once

#include "Component.h"
#include "BottomBar.h"
#include "Preset.h"
#include "UiDelegate.h"
#include "UiFeatures.h"

class PageView : public Component
{
public:
    PageView(const Preset &preset,
             UiDelegate *newDelegate,
             const UiFeatures &newUiFeatures,
             uint8_t newPageId,
             uint8_t activeControlSetId = 0);
    virtual ~PageView();

    void reassignComponent(const Control &control);
    void changePageName(const char *newName);

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    void addControls(const Controls &controls);
    void addGroups(const Groups &groups);
    void addBottomBar(const char *presetName, const char *pageName);

    const Preset &model;
    UiDelegate *delegate;
    const UiFeatures &uiFeatures;
    uint8_t pageId;
    uint8_t controlSetId;
    BottomBar *bottomBar;
};
