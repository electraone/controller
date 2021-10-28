#pragma once

#include "Component.h"
#include "Preset.h"

class PageView : public Component
{
public:
    PageView(const Preset &preset, uint8_t pageId);
    ~PageView() override;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    const Preset &model;
};
