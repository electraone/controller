#include "ControllerApp.h"

void Controller::printBounds(const Rectangle &bounds)
{
    logMessage("bounds: [%d %d %d %d]",
               bounds.getX(),
               bounds.getY(),
               bounds.getWidth(),
               bounds.getHeight());
}

void Controller::printPage(const Page &page)
{
    logMessage("id: %d", page.getId());
    logMessage("name: %s", page.getName());
    logMessage("hasObjects: %d", page.getHasObjects());
    logMessage("--");
}

void Controller::printDevice(const Device &device)
{
    logMessage("id: %d", device.getId());
    logMessage("name: %s", device.getName());
    logMessage("port: %d", device.getPort());
    logMessage("channel: %d", device.getChannel());
    logMessage("rate: %d", device.getRate());
    logMessage("requests: %d", device.requests.size());
    logMessage("responses: %d", device.responses.size());
    logMessage("postMessages: %d", device.postMessages.size());
    logMessage("--");
}

void Controller::printOverlay(const Overlay &overlay)
{
    logMessage("id: %d", overlay.getId());
    for (const auto &[midiValue, address] : overlay.items) {
        logMessage("    midiValue: %d, address: %s", midiValue, address);
    }
    logMessage("--");
}

void Controller::printGroup(const Group &group)
{
    logMessage("id: %d", group.getId());
    logMessage("label: %s", group.getLabel());
    logMessage("pageId: %d", group.getPageId());
    logMessage("colour: %d", group.getColour());
    printBounds(group.getBounds());
    logMessage("--");
}

void Controller::printInput(const Input &input)
{
    logMessage("    potId: %d", input.getPotId());
    logMessage("    valueId: %d", input.getValueId());
    logMessage("    --");
}

void Controller::printInputs(const std::vector<Input> &inputs)
{
    logMessage("    --[Inputs]------------------------------------");
    for (const auto &input : inputs) {
        printInput(input);
    }
}

void Controller::printValue(const Value2 &value)
{
    logMessage("    id: %s", value.getId());
    logMessage("    handle: %d", value.getHandle());
    logMessage("    index: %d", value.getIndex());
    logMessage("    default: %d", value.getDefault());
    logMessage("    min: %d", value.getMin());
    logMessage("    max: %d", value.getMax());
    logMessage("    overlayId: %d", value.getOverlayId());
    logMessage("    function: %s", value.getFunction());
    logMessage("    formatter: %s", value.getFormatter());
    printMessage(value.message);
    logMessage("    --");
}

void Controller::printValues(const std::vector<Value2> &values)
{
    logMessage("    --[Values]------------------------------------");
    for (const auto &value : values) {
        printValue(value);
    }
}

void Controller::printControl(const Control &control)
{
    if (control.getId() != 0) {
        logMessage("id: %d", control.getId());
        logMessage("type: %d", control.getType());
        logMessage("mode: %d", control.getMode());
        logMessage("variant: %d", control.getVariant());
        logMessage("visible: %d", control.getVisible());
        logMessage("name: %s", control.getName());
        logMessage("pageId: %d", control.getPageId());
        logMessage("colour: %d", control.getColour());
        logMessage("controlSetId: %d", control.getControlSetId());
        printBounds(control.getBounds());
        printInputs(control.inputs);
        printValues(control.values);

        logMessage("--");
    }
}

void Controller::printMessage(const Message &message)
{
    logMessage("        --[Message]--------------------------------");
    logMessage("        deviceId: %d", message.getDeviceId());
    logMessage("        type: %d", message.getType());
    logMessage("        parameterNumber: %d", message.getParameterNumber());
    logMessage("        min: %d", message.getMidiMin());
    logMessage("        max: %d", message.getMidiMax());
    logMessage("        value: %d", message.getValue());
    logMessage("        onValue: %d", message.getOnValue());
    logMessage("        offValue: %d", message.getOffValue());
    logMessage("        signMode: %d", message.getSignMode());
    logMessage("        lsbFirst: %d", message.getLsbFirst());
    logMessage("        bitWidth: %d", message.getBitWidth());
    logMessage("        --");
}
