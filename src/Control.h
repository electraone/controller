#pragma once

#include <vector>

#include "ElectraMessage.h"
#include "Value2.h"
#include "Input.h"
#include "Rectangle.h"

enum class ControlType {
    none = 0,
    fader = 1,
    vfader = 2,
    list = 3,
    pad = 4,
    adsr = 5,
    adr = 6,
    dx7envelope = 7
};

enum class ControlMode { none = 0, momentary = 1, toggle = 2 };

enum class Variant { automatic = 0, fixedValuePosition = 1 };

class ControlComponent;

class Control
{
public:
    Control();
    Control(uint16_t id,
            uint8_t pageId,
            const char *name,
            ControlType type,
            ControlMode mode,
            Colour colour,
            uint8_t controlSetId,
            Variant variant,
            bool visible);
    ~Control();

    uint16_t getId(void) const;
    uint8_t getPageId(void) const;
    ControlType getType(void) const;
    ControlMode getMode(void) const;
    Colour getColour(void) const;
    const char *getName(void) const;
    uint8_t getControlSetId(void) const;
    Variant getVariant(void) const;
    bool getVisible(void) const;
    Rectangle getBounds(void) const;
    void setBounds(Rectangle bounds);
    void setValues(std::vector<Value2> values);
    Value2 *getValue(const char *valueId);
    void assignControlComponent(ControlComponent *newControlComponent);
    ControlComponent *getControlComponent(void);

    static ControlType translateControlType(const char *typeText);
    static ControlMode translateControlMode(const char *modeText);
    static Variant translateVariant(const char *variantText);

private:
    static const int MaxNameLength = 15;

    Rectangle bounds;
    ControlComponent *controlComponent;

    struct {
        uint16_t id : 9;
        uint8_t pageId : 4;
        uint8_t type : 4;
        uint8_t mode : 2;
        uint8_t controlSetId : 3;
        uint8_t colour : 3;
        uint8_t variant : 2;
        uint8_t visible : 1;
    };

    char name[MaxNameLength + 1];

public:
    std::vector<Value2> values;
    std::vector<Input> inputs;
};
