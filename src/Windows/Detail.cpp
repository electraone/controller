#include "Detail.h"
#include "DetailFader.h"
#include "DetailList.h"
#include "DetailADSR.h"
#include "DetailADR.h"
#include "DetailDx7Env.h"

Detail::Detail(const Control &newControl, MainDelegate &newDelegate)
    : control(newControl),
      delegate(newDelegate),
      component(nullptr),
      lock(nullptr),
      locked(false)
{
    setName("detail");

    component =
        ControlComponent::createDetailControlComponent(control, delegate);

    if (component) {
        addAndMakeVisible(component);
    }

    lock = new Lock(locked);

    if (lock) {
        addAndMakeVisible(lock);

        lock->onClick = [this] {
            if (!locked) {
                delegate.lockDetail();
            } else {
                delegate.closeDetail();
            }
        };
    }

    dimmedSection = new VoidSpace();

    if (dimmedSection) {
        addAndMakeVisible(dimmedSection);

        dimmedSection->onClick = [this] {
            delegate.closeDetail();
        };
    }
}

void Detail::setLocked(void)
{
    locked = true;
    lock->setLocked(true);
}

bool Detail::isLocked(void)
{
    return (locked);
}

Detail *Detail::createDetail(const Control &control, MainDelegate &newDelegate)
{
    Detail *detail = nullptr;

    switch (control.getType()) {
        case ControlType::fader:
            detail = new DetailFader(control, newDelegate);
            break;

        case ControlType::list:
            detail = new DetailList(control, newDelegate);
            break;

        case ControlType::adsr:
            detail = new DetailADSR(control, newDelegate);
            break;

        case ControlType::adr:
            detail = new DetailADR(control, newDelegate);
            break;

        case ControlType::dx7envelope:
            detail = new DetailDx7Env(control, newDelegate);
            break;

        default:
            break;
    }

    return (detail);
}
