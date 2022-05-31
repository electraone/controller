#include "Detail.h"
#include "DetailFader.h"
#include "DetailList.h"
#include "DetailEnvelope.h"

Detail::Detail(const Control &newControl, UiDelegate *newDelegate)
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
                delegate->lockDetail();
            } else {
                delegate->closeDetail();
            }
        };
    }

    dimmedSection = new VoidSpace();

    if (dimmedSection) {
        addAndMakeVisible(dimmedSection);

        dimmedSection->onClick = [this] {
            delegate->closeDetail();
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

Detail *Detail::createDetail(const Control &control, UiDelegate *newDelegate)
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
        case ControlType::adr:
        case ControlType::dx7envelope:
            detail = new DetailEnvelope(control, newDelegate);
            break;

        default:
            break;
    }

    return (detail);
}
