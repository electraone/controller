#pragma once

#include "Component.h"
#include "Lock.h"
#include "VoidSpace.h"
#include "Model/Control.h"
#include "ControlComponent.h"
#include "System.h"
#include "UiDelegate.h"

class DetailFader;
class DetailList;
class DetailEnvelope;

class Detail : public Component
{
public:
    Detail(const Control &newControl, UiDelegate *newDelegate);
    virtual ~Detail() = default;

    void setLocked(void);
    bool isLocked(void);

    virtual void paint(Graphics &g) = 0;
    virtual void resized(void) = 0;

    static Detail *createDetail(const Control &control,
                                UiDelegate *newDelegate);

protected:
    const Control &control;
    UiDelegate *delegate;
    Component *component;
    Lock *lock;
    VoidSpace *dimmedSection;
    bool locked;
};
