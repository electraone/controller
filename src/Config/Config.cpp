#include "Config/Config.h"
#include "PersistentStorage.h"
#include "Hardware.h"
#include "System.h"

/** Config contructor
 *  an object that keeps information about the Electra base system settings that
 *  can be adjusted by the user
 */
Config::Config()
{
}

bool Config::load(void)
{
    return (load(System::context.getCurrentConfigFile()));
}

bool Config::load(const char *filename)
{
    File file;

    logMessage("Config::load: file: filename=%s", filename);

    file = Hardware::sdcard.createInputStream(filename);

    if (!file) {
        logMessage("Config::load: cannot open setup file: %s", filename);
        return (false);
    }

    file.setTimeout(100);

    if (!parse(file)) {
        logMessage("Config::load: cannot parse setup: filename=%s", filename);
        file.close();
        return (false);
    } else {
        file.close();
    }

    return (true);
}

void Config::serializeRoot(JsonDocument &doc)
{
    doc["version"] = 1;
}

void Config::serializeRouter(JsonDocument &doc)
{
    doc["router"]["usbDevToMidiIo"] = router.usbDevToMidiIo;
    doc["router"]["usbDevToUsbHost"] = router.usbDevToUsbHost;
    doc["router"]["midiIoToUsbDev"] = router.midiIoToUsbDev;
    doc["router"]["midiIoToUsbHost"] = router.midiIoToUsbHost;
    doc["router"]["usbHostToMidiIo"] = router.usbHostToMidiIo;
    doc["router"]["usbHostToUsbDev"] = router.usbHostToUsbDev;
    doc["router"]["midiIo1Thru"] = router.midiIo1Thru;
    doc["router"]["midiIo2Thru"] = router.midiIo2Thru;

    if (router.usbDevCtrl != Router::notSet) {
        doc["router"]["usbDevCtrl"] = router.usbDevCtrl;
    }

    if (router.midiControlChannel != Router::notSet) {
        doc["router"]["midiControlChannel"] = router.midiControlChannel;
    }
}

void Config::serializePresetBanks(JsonDocument &doc)
{
    JsonArray jPresetBanks = doc.createNestedArray("presetBanks");

    for (uint8_t i = 0; i < numPresetBanks; i++) {
        StaticJsonDocument<128> presetBankObject;
        JsonObject jPresetBank = presetBankObject.createNestedObject();

        jPresetBank["id"] = presetBanks[i].id;
        jPresetBank["name"] = presetBanks[i].name;
        jPresetBank["color"] = Colours::translateColour(presetBanks[i].colour);

        if (jPresetBanks.add(jPresetBank) == false) {
            logMessage(
                "Config::serialize: cannot add a presetBank to the setup");
        }
    }
}

void Config::serializeUsbHostAssigments(JsonDocument &doc)
{
    JsonArray jUsbHostAssigments = doc.createNestedArray("usbHostAssigments");

    for (auto &assigment : usbHostAssigments) {
        StaticJsonDocument<128> usbHostAssigmentObject;
        JsonObject jUsbHostAssigment =
            usbHostAssigmentObject.createNestedObject();

        jUsbHostAssigment["pattern"] = assigment.pattern;
        jUsbHostAssigment["port"] = assigment.port + 1;

        if (jUsbHostAssigments.add(jUsbHostAssigment) == false) {
            logMessage(
                "Config::serialize: cannot add a usbHostAssigment to the setup");
        }
    }
}

void Config::serializeMidiControl(JsonDocument &doc)
{
    JsonArray jMidiControls = doc.createNestedArray("midiControl");

    for (auto &midiControl : midiControls) {
        StaticJsonDocument<256> MidiControlObject;
        JsonObject jMidiControl = MidiControlObject.createNestedObject();

        jMidiControl["midiMessage"] =
            MidiMessage::translateTypeToText(midiControl.midiMessageType);
        jMidiControl["parameterNumber"] = midiControl.parameterNumber;

        if (!midiControl.command) {
            jMidiControl["event"] =
                translateAppEventTypeToText(midiControl.eventType);
            jMidiControl["eventParameter"] = midiControl.eventParameter1;
        } else {
            jMidiControl["command"]["type"] =
                translateAppEventTypeToText(midiControl.eventType);
            jMidiControl["command"]["pageId"] = midiControl.eventParameter1;
            jMidiControl["command"]["controlSetId"] =
                midiControl.eventParameter2;
        }

        if (jMidiControls.add(jMidiControl) == false) {
            logMessage(
                "Config::serialize: cannot add a midiControl to the setup");
        }
    }
}

void Config::serializeUiFeatures(JsonDocument &doc)
{
    doc["uiFeatures"]["touchSwitchControlSets"] =
        uiFeatures.touchSwitchControlSets;
    doc["uiFeatures"]["resetActiveControlSet"] =
        uiFeatures.resetActiveControlSet;
    doc["uiFeatures"]["activeControlSetType"] =
        translateControlSetTypeToText(uiFeatures.activeControlSetType);
}

void Config::serialize(void)
{
    StaticJsonDocument<2048> doc;

    File file = Hardware::sdcard.createOutputStream(
        System::context.getCurrentConfigFile(), FILE_WRITE | O_CREAT | O_TRUNC);

    if (!file) {
        logMessage("Config::serialize: cannot open setup file for writing");
        return;
    }

    serializeRoot(doc);
    serializeRouter(doc);
    serializePresetBanks(doc);
    serializeUsbHostAssigments(doc);
    serializeMidiControl(doc);
    serializeUiFeatures(doc);

    serializeJson(doc, file);

    file.close();
}

bool Config::parse(File &file)
{
    parseRouter(file);
    parsePresetBanks(file);
    parseUsbHostAssigments(file);
    parseMidiControl(file);
    parseUiFeatures(file);

    return (true);
}

bool Config::parseRouter(File &file)
{
    const size_t capacityRouter = JSON_OBJECT_SIZE(1) + 1000;
    const size_t capacityFilter = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacityRouter> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["router"] = true;

    if (file.seek(0) == false) {
        logMessage("Config::parseRouter: cannot rewind the file");
        return (false);
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Config::parseRouter: parsing failed: %s", err.c_str());
        return (false);
    }

    if (doc["router"]) {
        router.usbDevToUsbHost = doc["router"]["usbDevToUsbHost"].as<bool>();
        router.usbDevToMidiIo = doc["router"]["usbDevToMidiIo"].as<bool>();
        router.usbHostToUsbDev = doc["router"]["usbHostToUsbDev"].as<bool>();
        router.usbHostToMidiIo = doc["router"]["usbHostToMidiIo"].as<bool>();
        router.midiIoToUsbDev = doc["router"]["midiIoToUsbDev"].as<bool>();
        router.midiIoToUsbHost = doc["router"]["midiIoToUsbHost"].as<bool>();
        router.midiIo1Thru = doc["router"]["midiIo1Thru"].as<bool>();
        router.midiIo2Thru = doc["router"]["midiIo2Thru"].as<bool>();

        if (doc["router"].containsKey("usbDevCtrl")) {
            router.usbDevCtrl = doc["router"]["usbDevCtrl"].as<uint8_t>();
        } else {
            router.usbDevCtrl = Router::notSet;
        }

        if (doc["router"].containsKey("midiControlChannel")) {
            router.midiControlChannel =
                doc["router"]["midiControlChannel"].as<uint8_t>();
        } else {
            router.midiControlChannel = Router::notSet;
        }

        logMessage("Config::parseRouter: usbDevToUsbHost=%d",
                   router.usbDevToUsbHost);
        logMessage("Config::parseRouter: usbDevToMidiIo=%d",
                   router.usbDevToMidiIo);
        logMessage("Config::parseRouter: usbHostToUsbDev=%d",
                   router.usbHostToUsbDev);
        logMessage("Config::parseRouter: usbHostToMidiIo=%d",
                   router.usbHostToMidiIo);
        logMessage("Config::parseRouter: midiIoToUsbDev=%d",
                   router.midiIoToUsbDev);
        logMessage("Config::parseRouter: midiIoToUsbHost=%d",
                   router.midiIoToUsbHost);
        logMessage("Config::parseRouter: midiIo1Thru=%d", router.midiIo1Thru);
        logMessage("Config::parseRouter: midiIo2Thru=%d", router.midiIo2Thru);
        logMessage("Config::parseRouter: usbDevCtrl=%d", router.usbDevCtrl);
        logMessage("Config::parseRouter: midiControlChannel=%d",
                   router.midiControlChannel);
    } else {
        logMessage("Config::parseRouter: no router definition found");
    }

    return (true);
}

bool Config::parsePresetBanks(File &file)
{
    const size_t capacityPresetBanks = JSON_OBJECT_SIZE(1) + 1000;
    const size_t capacityFilter = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacityPresetBanks> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["presetBanks"] = true;

    resetPresetBanks();

    if (file.seek(0) == false) {
        logMessage("Config::parsePresetBanks: cannot rewind the file");
        return (false);
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Config::parsePresetBanks: parsing failed: %s", err.c_str());
        return (false);
    }

    JsonArray jPresetBanks = doc["presetBanks"];

    if (doc["presetBanks"]) {
        for (JsonVariant jPresetBank : jPresetBanks) {
            uint8_t id = jPresetBank["id"].as<uint8_t>();
            const char *name = jPresetBank["name"].as<char *>();
            const char *colourRGB888 = jPresetBank["color"].as<char *>();

            uint16_t colour = Colours::translateColour(colourRGB888);
            presetBanks[id - 1] = PresetBank(id, name, colour);

            logMessage("Config::parsePresetBanks: preset bank: id=%d, name=%s, "
                       "colour=%s",
                       id,
                       name,
                       colourRGB888);
        }
    } else {
        logMessage("Config::parsePresetBanks: no presetBanks definition found");
    }

    return (true);
}

bool Config::parseUsbHostAssigments(File &file)
{
    const size_t capacityAssigments = JSON_OBJECT_SIZE(1) + 1000;
    const size_t capacityFilter = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacityAssigments> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["usbHostAssigments"] = true;

    if (file.seek(0) == false) {
        logMessage("Config::parseUsbHostAssigments: cannot rewind the file");
        return (false);
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Config::parseUsbHostAssigments: parsing failed: %s",
                   err.c_str());
        return (false);
    }

    JsonArray jAssigments = doc["usbHostAssigments"];

    if (jAssigments) {
        usbHostAssigments = std::vector<UsbHostAssigment>();

        for (JsonVariant jAssigment : jAssigments) {
            const char *pattern = jAssigment["pattern"].as<char *>();
            uint8_t port = jAssigment["port"].as<uint8_t>();

            if (port > 0) {
                port--;
            }

            if (port > 2) {
                port = 0;
            }

            usbHostAssigments.push_back(UsbHostAssigment(pattern, port));
            logMessage(
                "Config::parseUsbHostAssigments: usb assigment: pattern=%s, "
                "port=%d",
                pattern,
                port);
        }
    } else {
        logMessage(
            "Config::parseUsbHostAssigments:: no usbHostAssigments definition found");
    }

    return (true);
}

bool Config::parseMidiControl(File &file)
{
    const size_t capacityMidiControls = JSON_OBJECT_SIZE(1) + 2000;
    const size_t capacityFilter = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacityMidiControls> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["midiControl"] = true;

    if (file.seek(0) == false) {
        logMessage("Config::parseMidiControl: cannot rewind the file");
        return (false);
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Config::parseMidiControl: parsing failed: %s", err.c_str());
        return (false);
    }

    JsonArray jMidiControls = doc["midiControl"];

    if (jMidiControls) {
        midiControls = std::vector<MidiControl>();

        for (JsonVariant jMidiControl : jMidiControls) {
            const char *midiMessage = jMidiControl["midiMessage"].as<char *>();
            uint8_t parameterNumber =
                jMidiControl["parameterNumber"].as<uint8_t>();

            if (jMidiControl["command"]) {
                const char *event =
                    jMidiControl["command"]["type"].as<char *>();
                uint8_t eventParameter1 =
                    jMidiControl["command"]["pageId"].as<uint8_t>();
                uint8_t eventParameter2 =
                    jMidiControl["command"]["controlSetId"].as<uint8_t>();

                if (eventParameter1 > 12) {
                    eventParameter1 = 0;
                }

                if (eventParameter2 > 3) {
                    eventParameter2 = 3;
                }

                if (parameterNumber > 127) {
                    parameterNumber = 0;
                }

                AppEventType eventType = translateAppEventType(event);
                MidiMessage::Type midiMessageType =
                    MidiMessage::translateType(midiMessage);

                midiControls.push_back(MidiControl(eventType,
                                                   eventParameter1,
                                                   eventParameter2,
                                                   midiMessageType,
                                                   parameterNumber));
                logMessage(
                    "Config::parseMidiControl: midi control assigment: "
                    "event=%s (%d), eventParameter1=%d, eventParameter2=%d, "
                    "midiMessage=%s (%d), parameterNumber=%d",
                    event,
                    eventType,
                    eventParameter1,
                    eventParameter2,
                    midiMessage,
                    midiMessageType,
                    parameterNumber);
            } else {
                const char *event = jMidiControl["event"].as<char *>();
                uint8_t eventParameter =
                    jMidiControl["eventParameter"].as<uint8_t>();

                if (eventParameter > 12) {
                    eventParameter = 0;
                }

                if (parameterNumber > 127) {
                    parameterNumber = 0;
                }

                AppEventType eventType = translateAppEventType(event);
                MidiMessage::Type midiMessageType =
                    MidiMessage::translateType(midiMessage);

                midiControls.push_back(MidiControl(eventType,
                                                   eventParameter,
                                                   midiMessageType,
                                                   parameterNumber));
                logMessage(
                    "Config::parseMidiControl: midi control assigment: "
                    "event=%s (%d), eventParameter=%d, midiMessage=%s (%d), "
                    "parameterNumber=%d",
                    event,
                    eventType,
                    eventParameter,
                    midiMessage,
                    midiMessageType,
                    parameterNumber);
            }
        }
    } else {
        logMessage(
            "Config::parseMidiControl:: no midiControl definition found");
    }

    return (true);
}

bool Config::parseUiFeatures(File &file)
{
    const size_t capacityUiFeatures = JSON_OBJECT_SIZE(1) + 1000;
    const size_t capacityFilter = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacityUiFeatures> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["uiFeatures"] = true;

    if (file.seek(0) == false) {
        logMessage("Config::parseUiFeatures: cannot rewind the file");
        return (false);
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Config::parseUiFeatures: parsing failed: %s", err.c_str());
        return (false);
    }

    if (doc["uiFeatures"]) {
        uiFeatures.touchSwitchControlSets =
            doc["uiFeatures"]["touchSwitchControlSets"].as<bool>();
        logMessage("Config::parseUiFeatures: touchSwitchControlSets=%d",
                   uiFeatures.touchSwitchControlSets);
        uiFeatures.resetActiveControlSet =
            doc["uiFeatures"]["resetActiveControlSet"].as<bool>();
        logMessage("Config::parseUiFeatures: resetActiveControlSet=%d",
                   uiFeatures.resetActiveControlSet);
        uiFeatures.activeControlSetType = translateControlSetType(
            doc["uiFeatures"]["activeControlSetType"].as<char *>());
        logMessage("Config::parseUiFeatures: activeControlSetType=%d",
                   uiFeatures.activeControlSetType);
    } else {
        logMessage("Config::parseUiFeatures: no UiFeatures definition found");
    }

    return (true);
}

void Config::resetPresetBanks(void)
{
    presetBanks[0] = PresetBank(1, "BANK #1", Colours::white);
    presetBanks[1] = PresetBank(2, "BANK #2", Colours::red);
    presetBanks[2] = PresetBank(3, "BANK #3", Colours::orange);
    presetBanks[3] = PresetBank(4, "BANK #4", Colours::blue);
    presetBanks[4] = PresetBank(5, "BANK #5", Colours::green);
    presetBanks[5] = PresetBank(6, "BANK #6", Colours::purple);
}

void Config::resetUiFeatures(void)
{
    uiFeatures.touchSwitchControlSets = true;
    uiFeatures.resetActiveControlSet = true;
    uiFeatures.activeControlSetType = ActiveControlSetType::dim;
}

void Config::useDefault(void)
{
    resetPresetBanks();
    resetUiFeatures();
    serialize();
}

uint8_t Config::getUsbHostAssigment(const char *productName)
{
    char productNameUpperCase[50];

    copyString(
        productNameUpperCase, productName, sizeof(productNameUpperCase) - 1);
    toUpperCase(productNameUpperCase);

    for (auto &assigment : usbHostAssigments) {
        if (strstr(productNameUpperCase, assigment.pattern)) {
            logMessage("Config::getUsbHostAssigment: assigning to midi bus: "
                       "device=%s, midiBus=%d",
                       productName,
                       assigment.port);
            return (assigment.port);
        }
    }
    return (0);
}
