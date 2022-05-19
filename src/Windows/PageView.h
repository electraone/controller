#pragma once

#include "Component.h"
#include "Preset.h"

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
    const Preset &model;
    uint8_t pageId;
};
