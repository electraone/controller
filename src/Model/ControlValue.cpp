#include "ControlValue.h"
#include "Control.h"

static const char *valueIdsAdsr[] = { "attack", "decay", "sustain", "release" };

static const char *valueIdsAdr[] = { "attack", "decay", "release" };

static const char *valueIdsDx7Env[] = { "l1", "r1", "l2", "r2",
                                        "l3", "r3", "l4", "r4" };

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

Overlay *ControlValue::getOverlay(void) const
{
    return (overlay);
}

uint16_t ControlValue::getNumSteps(void) const
{
    return (max - min);
}

const char *ControlValue::ControlValue::getFunction(void) const
{
    return (luaFunctions[function].c_str());
}

const std::string ControlValue::getFormatter(void) const
{
    return (luaFunctions[formatter]);
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
        case ControlType::fader:
        case ControlType::list:
        case ControlType::pad:
            return ("value");
            break;

        case ControlType::adsr:
            if (id < 4) {
                return (valueIdsAdsr[id]);
            }
            break;

        case ControlType::adr:
            if (id < 3) {
                return (valueIdsAdr[id]);
            }
            break;

        case ControlType::dx7envelope:
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
    switch (control->getType()) {
        case ControlType::fader:
        case ControlType::list:
        case ControlType::pad:
            return (0);
            break;

        case ControlType::adsr:
            for (uint8_t i = 0; i < 4; i++) {
                if (strcmp(handle, valueIdsAdsr[i]) == 0) {
                    return (i);
                }
            }
            break;
        case ControlType::adr:
            for (uint8_t i = 0; i < 3; i++) {
                if (strcmp(handle, valueIdsAdr[i]) == 0) {
                    return (i);
                }
            }
            break;

        case ControlType::dx7envelope:
            for (uint8_t i = 0; i < 8; i++) {
                if (strcmp(handle, valueIdsDx7Env[i]) == 0) {
                    return (i);
                }
            }
            break;

        default:
            break;
    }
    return (0);
}

void ControlValue::print(void) const
{
    logMessage("    id: %s", getId());
    logMessage("    handle: %d", getHandle());
    logMessage("    index: %d", getIndex());
    logMessage("    default: %d", getDefault());
    logMessage("    min: %d", getMin());
    logMessage("    max: %d", getMax());
    logMessage("    overlayId: %d", getOverlayId());
    logMessage("    function: %s", getFunction());
    logMessage("    formatter: %s", getFormatter().c_str());
    logMessage("    overlay: %x", getOverlay());
    logMessage("    control: %x", getControl());
    message.print();
}
