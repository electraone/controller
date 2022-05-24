#pragma once

#include "Component.h"
#include "Preset.h"
#include "System.h"

class PageView : public Component
{
public:
    PageView(const Preset &preset,
             uint8_t pageId,
             uint8_t activeControlSetId = 0);
    virtual ~PageView() override;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    void addControls(const Controls &controls,
                     uint8_t pageId,
                     uint8_t activeControlSetId);
    void addGroups(const Groups &groups,
                   uint8_t pageId,
                   uint8_t activeControlSetId);
    void addBottomBar(const char *presetName, const char *pageName);

    void repaintParameterMap(void);
    void enableRepaintParameterMap(void);
    void disableRepaintParameterMap(void);

    const Preset &model;
    Task repaintParameterMapTask;
};
