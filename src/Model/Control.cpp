#include "Control.h"

Control::Control() : id(0), pageId(0), controlSetId(0)
{
    type = (uint8_t)ControlType::none;
    colour = (uint8_t)ElectraColours::white;
    mode = (uint8_t)ControlMode::none;
    controlSetId = 0;
    variant = (uint8_t)Variant::automatic;
    visible = (uint8_t) false;
    name[0] = '\0';
}

Control::Control(uint16_t id,
                 uint8_t newPageId,
                 const char *newName,
                 const Rectangle &newBounds,
                 ControlType newType,
                 ControlMode newMode,
                 Colour newColour,
                 uint8_t newControlSetId,
                 Variant newVariant,
                 bool newVisible)
    : id(id),
      pageId(newPageId),
      bounds(newBounds),
      controlSetId(newControlSetId)
{
    copyString(name, newName, MaxNameLength);
    type = (uint8_t)newType;
    colour = (uint8_t)newColour;
    mode = (uint8_t)newMode;
    variant = (uint8_t)newVariant;
    visible = (uint8_t)newVisible;
}

uint16_t Control::getId(void) const
{
    return (id);
}

uint8_t Control::getPageId(void) const
{
    return (pageId);
}

ControlType Control::getType(void) const
{
    return ((ControlType)type);
}

ControlMode Control::getMode(void) const
{
    return ((ControlMode)mode);
}

Colour Control::getColour(void) const
{
    return (colour);
}

const char *Control::getName(void) const
{
    return (name);
}

uint8_t Control::getControlSetId(void) const
{
    return (controlSetId);
}

Variant Control::getVariant(void) const
{
    return ((Variant)variant);
}

bool Control::getVisible(void) const
{
    return ((bool)visible);
}

Rectangle Control::getBounds(void) const
{
    return (bounds);
}

void Control::setBounds(const Rectangle &newBounds)
{
    bounds = newBounds;
}

void Control::setValues(std::vector<ControlValue> newValues)
{
    values = newValues;
}
/*
ControlValue *Control::getValue(const char *valueId)
{
    uint8_t handle = ControlValue::translateId(valueId);

    for (auto &value : values) {
        if (value.getHandle() == handle) {
            return (&value);
        }
    }

    return (nullptr);
}
*/
const ControlValue &Control::getValue(uint8_t index) const
{
    return (values[index]);
}

ControlType Control::translateControlType(const char *typeText)
{
    if (typeText) {
        if (strcmp(typeText, "fader") == 0) {
            return (ControlType::fader);
        } else if (strcmp(typeText, "vfader") == 0) {
            return (ControlType::vfader);
        } else if (strcmp(typeText, "list") == 0) {
            return (ControlType::list);
        } else if (strcmp(typeText, "pad") == 0) {
            return (ControlType::pad);
        } else if (strcmp(typeText, "adsr") == 0) {
            return (ControlType::adsr);
        } else if (strcmp(typeText, "adr") == 0) {
            return (ControlType::adr);
        } else if (strcmp(typeText, "dx7envelope") == 0) {
            return (ControlType::dx7envelope);
        }
    }
    return (ControlType::none);
}

ControlMode Control::translateControlMode(const char *modeText)
{
    if (modeText) {
        if (strcmp(modeText, "momentary") == 0) {
            return (ControlMode::momentary);
        } else if (strcmp(modeText, "toggle") == 0) {
            return (ControlMode::toggle);
        }
    }

    return (ControlMode::none);
}

Variant Control::translateVariant(const char *variantText)
{
    if (variantText) {
        if (strcmp(variantText, "fixedValuePosition") == 0) {
            return (Variant::fixedValuePosition);
        }
    }

    return (Variant::automatic);
}

void Control::print(void) const
{
    if (getId() != 0) {
        logMessage("id: %d", getId());
        logMessage("type: %d", getType());
        logMessage("mode: %d", getMode());
        logMessage("variant: %d", getVariant());
        logMessage("visible: %d", getVisible());
        logMessage("name: %s", getName());
        logMessage("pageId: %d", getPageId());
        logMessage("colour: %d", getColour());
        logMessage("controlSetId: %d", getControlSetId());
        getBounds().print();
        printInputs();
        printValues();
    }
}

void Control::printValues(void) const
{
    logMessage("    --[Values]------------------------------------");
    for (const auto &value : values) {
        value.print();
    }
}

void Control::printInputs(void) const
{
    logMessage("    --[Inputs]------------------------------------");
    for (const auto &input : inputs) {
        input.print();
    }
}
