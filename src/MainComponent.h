#pragma once

#include "Component.h"


class MainComponent : public Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(Graphics &g) override;
    void resized(void) override;
};
