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
#include "MainWindow.h"

ControlComponent::ControlComponent(const Control &controlToAssign)
    : control(controlToAssign)
{
}

void ControlComponent::onTouchLongHold(const TouchEvent &touchEvent)
{
    logMessage("opeing detail");
    if (MainWindow *window = dynamic_cast<MainWindow *>(getWindow())) {
        window->openDetail(getId());
    }
}

void ControlComponent::emitValueChange(int16_t newDisplayValue,
                                       const ControlValue &cv)
{
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

Component *ControlComponent::createControlComponent(const Control &control,
                                                    bool detail)
{
    Component *c = nullptr;

    if (detail == true) {
        if (control.getType() == ControlType::fader) {
            c = new FaderDetailControl(control);
        } else if (control.getType() == ControlType::list) {
            c = new ListDetailControl(control);
        } else if (control.getType() == ControlType::adsr) {
            c = new ADSRDetailControl(control);
        } else if (control.getType() == ControlType::adr) {
            c = new ADRDetailControl(control);
        } else if (control.getType() == ControlType::dx7envelope) {
            c = new Dx7EnvDetailControl(control);
        }
    } else {
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
    }

    if (c) {
        c->setBounds(control.getBounds());
        c->setName(control.getName());
        c->setId(control.getId());
        c->setVisible(control.getVisible());
    }

    return (c);
}
