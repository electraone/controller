#pragma once

#include <cstdint>
#include <cstring>
#include <string>

#include "ElectraMessage.h"
#include "Overlay.h"
#include "Message.h"
#include "Macros.h"
#include "helpers.h"
#include "luahooks.h"

class Control;

class ControlValue
{
public:
    ControlValue();
    ControlValue(Control *newControl,
                 const char *newValueId,
                 uint8_t newIndex,
                 int16_t newDefaultValue,
                 int16_t newMin,
                 int16_t newMax,
                 uint8_t newOverlayId,
                 Message(newMessage),
                 const char *newFormatter,
                 const char *newFunction,
                 Overlay *newOverlay);

    virtual ~ControlValue() = default;

    Control *getControl(void) const;
    int16_t getHandle(void) const;
    int16_t getIndex(void) const;
    const char *getId(void) const;
    void setDefault(int16_t newDefaultValue);
    int16_t getDefault(void) const;
    void setMin(int16_t newMin);
    int16_t getMin(void) const;
    void setMax(int16_t newMax);
    int16_t getMax(void) const;
    void setOverlayId(uint8_t newOverlayId);
    uint8_t getOverlayId(void) const;
    Overlay *getOverlay(void) const;
    uint16_t getNumSteps(void) const;
    const char *getFunction(void) const;
    std::string getFormatter(void) const;
    void callFormatter(int16_t value, char *buffer, size_t length) const;
    void callFunction(int16_t value) const;
    const char *translateId(uint8_t id) const;
    uint8_t translateId(const char *handle) const;
    void print(void) const;

    /*
	 * attributes
	 */
private:
    uint8_t handle;
    uint8_t index;
    int16_t defaultValue;
    int16_t min;
    int16_t max;
    uint8_t overlayId;
    std::string formatter;
    std::string function;
    Control *control;
    Overlay *overlay;

public:
    Message message;
};
