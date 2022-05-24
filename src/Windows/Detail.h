#pragma once

#include "Component.h"
#include "Model/Control.h"
#include "UiDelegate.h"

class Detail : public Component
{
public:
    Detail(const Control &control, UiDelegate *newDelegate);
    ~Detail() override;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    const Control &control;
    UiDelegate *delegate;
};
