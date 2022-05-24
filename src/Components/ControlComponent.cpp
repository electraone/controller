#include "ControlComponent.h"
#include "FaderControl.h"
#include "ListControl.h"
#include "PadControl.h"
#include "ADSRControl.h"
#include "ADRControl.h"
#include "DX7EnvControl.h"
#include "MainWindow.h"

ControlComponent::ControlComponent(const Control &controlToAssign)
    : control(controlToAssign)
{
}

void ControlComponent::onTouchLongHold(const TouchEvent &touchEvent)
{
    if (MainWindow *window = dynamic_cast<MainWindow *>(getWindow())) {
        window->displayDetail(getId());
    }
}

Component *ControlComponent::createControlComponent(const Control &control)
{
    Component *c = nullptr;

    if (control.getType() == ControlType::fader) {
        c = new FaderControl(control);
    } else if (control.getType() == ControlType::list) {
        c = new ListControl(control);
    } else if (control.getType() == ControlType::pad) {
        c = new PadControl(control);
    } else if (control.getType() == ControlType::adsr) {
        c = new ADSRControl(control);
    } else if (control.getType() == ControlType::adr) {
        c = new ADRControl(control);
    } else if (control.getType() == ControlType::dx7envelope) {
        c = new Dx7EnvControl(control);
    }

    if (c) {
        c->setBounds(control.getBounds());
        c->setName(control.getName());
        c->setId(control.getId());
        c->setVisible(control.getVisible());
    }

    return (c);
}
