#include "ControlComponent.h"
#include "FaderControl.h"
#include "ListControl.h"
#include "PadControl.h"
#include "ADSRControl.h"
#include "ADRControl.h"
#include "DX7EnvControl.h"
#include "FaderDetailControl.h"
#include "ListDetailControl.h"
#include "ADSRDetailControl.h"
#include "ADRDetailControl.h"
#include "Dx7EnvDetailControl.h"
#include "KnobControl.h"
#include "MainWindow.h"

ControlComponent::ControlComponent(const Control &controlToAssign,
                                   UiDelegate *newDelegate)
    : control(controlToAssign), delegate(newDelegate)
{
}

void ControlComponent::onControlUpdated(void)
{
    Component *c = this;

    c->setBounds(control.getBounds());
    c->setName(control.getName());
    c->setId(control.getId());
    c->setVisible(control.isVisible());
}

void ControlComponent::onTouchLongHold(const TouchEvent &touchEvent)
{
    if (MainWindow *window = dynamic_cast<MainWindow *>(getWindow())) {
        window->openDetail(getId());
    }
}

void ControlComponent::onTouchDoubleClick(const TouchEvent &touchEvent)
{
    uint8_t handle = 0;

    if (Envelope *en = dynamic_cast<Envelope *>(this)) {
        handle = en->getActiveSegment();
    }
    delegate->setDefaultValue(getId(), handle);
}

void ControlComponent::updateValueFromParameterMap(void)
{
    for (const auto &value : control.values) {
        const auto &message = value.message;
        uint16_t midiValue =
            parameterMap.getValue(message.getDeviceId(),
                                  message.getType(),
                                  message.getParameterNumber());
        onMidiValueChange(value, midiValue, value.getHandle());
    }
}

void ControlComponent::emitValueChange(int16_t newDisplayValue,
                                       const ControlValue &cv)
{
    newDisplayValue = constrain(newDisplayValue, cv.getMin(), cv.getMax());

    uint16_t midiValue = translateValueToMidiValue(cv.message.getSignMode(),
                                                   cv.message.getBitWidth(),
                                                   newDisplayValue,
                                                   cv.getMin(),
                                                   cv.getMax(),
                                                   cv.message.getMidiMin(),
                                                   cv.message.getMidiMax());

    parameterMap.setValue(cv.message.getDeviceId(),
                          cv.message.getType(),
                          cv.message.getParameterNumber(),
                          midiValue,
                          Origin::internal);

    cv.callFunction(newDisplayValue);
#ifdef DEBUG
    logMessage(
        "emitValueChange: display=%d, midi=%d", newDisplayValue, midiValue);
#endif
}

ControlComponent *
    ControlComponent::createControlComponent(const Control &control,
                                             UiDelegate *newDelegate)
{
    ControlComponent *c = nullptr;

    if (control.getType() == ControlType::fader) {
        c = new FaderControl(control, newDelegate);
    } else if (control.getType() == ControlType::list) {
        c = new ListControl(control, newDelegate);
    } else if (control.getType() == ControlType::pad) {
        c = new PadControl(control, newDelegate);
    } else if (control.getType() == ControlType::adsr) {
        c = new ADSRControl(control, newDelegate);
    } else if (control.getType() == ControlType::adr) {
        c = new ADRControl(control, newDelegate);
    } else if (control.getType() == ControlType::dx7envelope) {
        c = new Dx7EnvControl(control, newDelegate);
    } else if (control.getType() == ControlType::knob) {
        c = new KnobControl(control, newDelegate);
    }

    if (c) {
        c->onControlUpdated();
    }

    return (c);
}

ControlComponent *
    ControlComponent::createDetailControlComponent(const Control &control,
                                                   UiDelegate *newDelegate)
{
    ControlComponent *c = nullptr;

    if (control.getType() == ControlType::fader) {
        c = new FaderDetailControl(control, newDelegate);
    } else if (control.getType() == ControlType::list) {
        c = new ListDetailControl(control, newDelegate);
    } else if (control.getType() == ControlType::adsr) {
        c = new ADSRDetailControl(control, newDelegate);
    } else if (control.getType() == ControlType::adr) {
        c = new ADRDetailControl(control, newDelegate);
    } else if (control.getType() == ControlType::dx7envelope) {
        c = new Dx7EnvDetailControl(control, newDelegate);
    }

    if (c) {
        c->setName(control.getName());
        c->setId(control.getId());
    }

    return (c);
}
