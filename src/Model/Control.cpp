#include "Control.h"
#include "ParameterMap.h"

static const char *valueIdsAdsr[] = { "attack", "decay", "sustain", "release" };

static const char *valueIdsAdr[] = { "attack", "decay", "release" };

static const char *valueIdsDx7Env[] = { "l1", "r1", "l2", "r2",
                                        "l3", "r3", "l4", "r4" };

Control::Control() : id(0), pageId(0), controlSetId(0), component(nullptr)
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
      controlSetId(newControlSetId),
      component(nullptr)
{
    copyString(name, newName, MaxNameLength);
    type = (uint8_t)newType;
    colour = (uint8_t)newColour;
    mode = (uint8_t)newMode;
    variant = (uint8_t)newVariant;
    visible = (uint8_t)newVisible;
}

bool Control::isValid(void) const
{
    return (id != 0);
}

void Control::setId(uint16_t newId)
{
    id = newId;
}

uint16_t Control::getId(void) const
{
    return (id);
}

uint8_t Control::getPageId(void) const
{
    return (pageId);
}

void Control::setType(ControlType newType)
{
    type = (uint8_t)newType;
}

ControlType Control::getType(void) const
{
    return ((ControlType)type);
}

ControlMode Control::getMode(void) const
{
    return ((ControlMode)mode);
}

void Control::setColour(Colour newColour)
{
    colour = newColour;
}

Colour Control::getColour(void) const
{
    return (colour);
}

void Control::setName(const char *newName)
{
    copyString(name, newName, MaxNameLength);
}

const char *Control::getName(void) const
{
    return (name);
}

void Control::setControlSetId(uint8_t newControlSetId)
{
    controlSetId = newControlSetId;
}

uint8_t Control::getControlSetId(void) const
{
    return (controlSetId);
}

Variant Control::getVariant(void) const
{
    return ((Variant)variant);
}

void Control::setVisible(bool shouldBeVisible)
{
    visible = shouldBeVisible;
}

bool Control::isVisible(void) const
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

ControlValue &Control::getValue(uint8_t index)
{
    return (values[index]);
}

const ControlValue &Control::getValue(uint8_t index) const
{
    return (values[index]);
}

const ControlValue &Control::getValueByValueId(const char *valueId) const
{
    return (values[0]);
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

const char *Control::translateValueId(uint8_t id) const
{
    switch (getType()) {
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

uint8_t Control::translateValueId(const char *handle) const
{
    switch (getType()) {
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

void Control::setComponent(Component *newComponent)
{
    component = newComponent;
}

void Control::resetComponent(void)
{
    component = nullptr;
}

Component *Control::getComponent(void) const
{
    return (component);
}

void Control::addToParameterMap(ControlValue &value)
{
#ifdef DEBUG
    logMessage("addToParameterMap: controlId: %d, handle: %d, address=%x",
               value.getControl()->getId(),
               value.getHandle(),
               &value);
#endif
    LookupEntry *lookupEntry =
        parameterMap.getOrCreate(value.message.getDeviceId(),
                                 value.message.getType(),
                                 value.message.getParameterNumber());
    if (lookupEntry) {
        lookupEntry->messageDestination.push_back(&value);
    }
}

void Control::removeFromParameterMap(ControlValue &value)
{
    LookupEntry *lookupEntry =
        parameterMap.getOrCreate(value.message.getDeviceId(),
                                 value.message.getType(),
                                 value.message.getParameterNumber());
    if (lookupEntry) {
        for (uint16_t i = 0; i < lookupEntry->messageDestination.size(); i++) {
            if (lookupEntry->messageDestination[i] == &value) {
                lookupEntry->messageDestination.erase(
                    lookupEntry->messageDestination.begin() + i);
                break;
            }
        }
    }
}

void Control::setDefaultValue(ControlValue &value, bool sendMidiMessages)
{
    // \todo replace this with polymorphism
    if (value.message.getType() == ElectraMessageType::start) {
        parameterMap.getOrCreate(0xff, value.message.getType(), 0)
            ->messageDestination.push_back(&value);
    } else if (value.message.getType() == ElectraMessageType::stop) {
        parameterMap.getOrCreate(0xff, value.message.getType(), 0)
            ->messageDestination.push_back(&value);
    } else if (value.message.getType() == ElectraMessageType::tune) {
        parameterMap.getOrCreate(0xff, value.message.getType(), 0)
            ->messageDestination.push_back(&value);
    } else {
        LookupEntry *lookupEntry =
            parameterMap.getOrCreate(value.message.getDeviceId(),
                                     value.message.getType(),
                                     value.message.getParameterNumber());
        lookupEntry->messageDestination.push_back(&value);

        int16_t midiValue = 0;

        if (getType() == ControlType::pad) {
            midiValue = value.getDefault();
        } else if (getType() == ControlType::list) {
            midiValue = value.getDefault();
        } else {
            midiValue = translateValueToMidiValue(value.message.getSignMode(),
                                                  value.message.getBitWidth(),
                                                  value.getDefault(),
                                                  value.getMin(),
                                                  value.getMax(),
                                                  value.message.getMidiMin(),
                                                  value.message.getMidiMax());
        }
        parameterMap.setValue(value.message.getDeviceId(),
                              value.message.getType(),
                              value.message.getParameterNumber(),
                              midiValue,
                              sendMidiMessages ? Origin::internal
                                               : Origin::file);
    }
}

void Control::print(void) const
{
    if (isValid()) {
        logMessage("address: %x", this);
        logMessage("id: %d", getId());
        logMessage("type: %d", getType());
        logMessage("mode: %d", getMode());
        logMessage("variant: %d", getVariant());
        logMessage("visible: %d", isVisible());
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
