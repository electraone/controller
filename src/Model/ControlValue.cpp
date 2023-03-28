#include "ControlValue.h"
#include "Control.h"

// \todo this is a brute way of reducing memory usage by the ControlValue
extern std::vector<std::string> luaFunctions;

ControlValue::ControlValue()
    : handle(0),
      index(0),
      overlayId(0),
      defaultValue(0),
      min(0),
      max(NOT_SET),
      control(nullptr),
      overlay(nullptr)
{
    label[0] = '\0';
}

ControlValue::ControlValue(Control *newControl,
                           const char *newValueId,
                           uint8_t newIndex,
                           int16_t newDefaultValue,
                           int16_t newMin,
                           int16_t newMax,
                           uint8_t newOverlayId,
                           Message(newMessage),
                           uint8_t newFormatter,
                           uint8_t newFunction,
                           Overlay *newOverlay)
    : control(newControl),
      index(newIndex),
      defaultValue(newDefaultValue),
      min(newMin),
      max(newMax),
      overlayId(newOverlayId),
      message(newMessage),
      formatter(newFormatter),
      function(newFunction),
      overlay(newOverlay)
{
    // translate the valueId to the numeric handle
    handle = translateId(newValueId);
    label[0] = '\0';
}

void ControlValue::setControl(Control *newControl)
{
    control = newControl;
}

Control *ControlValue::getControl(void) const
{
    return (control);
}

int16_t ControlValue::getHandle(void) const
{
    return (handle);
}

int16_t ControlValue::getIndex(void) const
{
    return (index);
}

const char *ControlValue::getId(void) const
{
    return (translateId(handle));
}

void ControlValue::setDefault(int16_t newDefaultValue)
{
    defaultValue = newDefaultValue;
}

int16_t ControlValue::getDefault(void) const
{
    return (defaultValue);
}

void ControlValue::setMin(int16_t newMin)
{
    min = newMin;
}

int16_t ControlValue::getMin(void) const
{
    return (min);
}

void ControlValue::setMax(int16_t newMax)
{
    max = newMax;
}

int16_t ControlValue::getMax(void) const
{
    return (max);
}

void ControlValue::setOverlayId(uint8_t newOverlayId)
{
    overlayId = newOverlayId;
}

uint8_t ControlValue::getOverlayId(void) const
{
    return (overlayId);
}

void ControlValue::setOverlay(Overlay *newOverlay)
{
    overlay = newOverlay;
}

Overlay *ControlValue::getOverlay(void) const
{
    return (overlay);
}

uint16_t ControlValue::getNumSteps(void) const
{
    return (max - min);
}

bool ControlValue::isFunctionAssigned(void) const
{
    return (!luaFunctions[function].empty());
}

bool ControlValue::isFormatterAssigned(void) const
{
    return (!luaFunctions[formatter].empty());
}

const char *ControlValue::ControlValue::getFunction(void) const
{
    return (luaFunctions[function].c_str());
}

const std::string ControlValue::getFormatter(void) const
{
    return (luaFunctions[formatter]);
}

void ControlValue::setLabel(const char *newLabel)
{
    copyString(label, newLabel, MaxLabelLength);
}

void ControlValue::resetLabel(void)
{
    label[0] = '\0';
}

const char *ControlValue::getLabel(void) const
{
    return (label);
}

bool ControlValue::isLabelSet(void) const
{
    return (label[0] != '\0');
}

void ControlValue::callFormatter(int16_t value,
                                 char *buffer,
                                 size_t length) const
{
    if (L != nullptr && !luaFunctions[formatter].empty()) {
        runFormatter(
            luaFunctions[formatter].c_str(), this, value, buffer, length);
    }
}

void ControlValue::callFunction(int16_t value) const
{
    if (L != nullptr && (value != MIDI_VALUE_DO_NOT_SEND)
        && !luaFunctions[function].empty()) {
        runFunction(luaFunctions[function].c_str(), this, value);
    }
}

const char *ControlValue::translateId(uint8_t id) const
{
    switch (control->getType()) {
        case Control::Type::Fader:
        case Control::Type::List:
        case Control::Type::Pad:
            return ("value");
            break;

        case Control::Type::Adsr:
            if (id < 4) {
                return (valueIdsAdsr[id]);
            }
            break;

        case Control::Type::Adr:
            if (id < 3) {
                return (valueIdsAdr[id]);
            }
            break;

        case Control::Type::Dx7envelope:
            if (id < 8) {
                return (valueIdsDx7Env[id]);
            }
            break;

        default:
            break;
    }
    return ("value");
}

uint8_t ControlValue::translateId(const char *handle) const
{
    return (Control::translateValueToId(control->getType(), handle));
}

int16_t ControlValue::translateMidiValue(uint16_t midiValue) const
{
    if (control->getType() == Control::Type::List) {
        return (overlay->getIndexByValue(midiValue));
    } else if (control->getType() == Control::Type::Pad) {
        return (midiValue == message.getOnValue());
    }
    return (translateMidiValueToValue(message.getSignMode(),
                                      message.getBitWidth(),
                                      midiValue,
                                      message.getMidiMin(),
                                      message.getMidiMax(),
                                      getMin(),
                                      getMax()));
}

void ControlValue::print(void) const
{
    System::logger.write(LOG_ERROR, "    id: %s", getId());
    System::logger.write(LOG_ERROR, "    handle: %d", getHandle());
    System::logger.write(LOG_ERROR, "    index: %d", getIndex());
    System::logger.write(LOG_ERROR, "    default: %d", getDefault());
    System::logger.write(LOG_ERROR, "    min: %d", getMin());
    System::logger.write(LOG_ERROR, "    max: %d", getMax());
    System::logger.write(LOG_ERROR, "    overlayId: %d", getOverlayId());
    System::logger.write(LOG_ERROR, "    function: %s", getFunction());
    System::logger.write(
        LOG_ERROR, "    formatter: %s", getFormatter().c_str());
    System::logger.write(LOG_ERROR, "    overlay: %x", getOverlay());
    System::logger.write(LOG_ERROR, "    control: %x", getControl());
    message.print();
}
