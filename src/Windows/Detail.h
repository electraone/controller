#pragma once

#include "Component.h"
#include "Model/Control.h"
#include "System.h"
#include "UiDelegate.h"

class Detail : public Component
{
public:
    Detail(const Control &control, UiDelegate *newDelegate);
    virtual ~Detail() = default;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    const Control &control;
    UiDelegate *delegate;
};
