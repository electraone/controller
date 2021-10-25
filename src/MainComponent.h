#pragma once

#include "Component.h"
#include "ButtonListener.h"
#include "Preset.h"

class MainComponent : public Component, public ButtonListener
{
public:
    MainComponent(Preset &preset);
    ~MainComponent() override;

    void paint(Graphics &g) override;
    void resized(void) override;

    void onButtonDown(uint8_t buttonId) override;
    void displayPage(uint8_t pageId);

private:
    Preset &model;
    uint8_t currentPage;
};
