#include "App.h"
#include "MainComponent.h"
#include "Preset.h"

extern MemoryPool stringPool;
extern BitmapPool bitmapPool;

class Controller : public App
{
public:
    Controller()
    {
        windowManager.setActiveWindow(&mainWindow);
    }

    const char *getApplicationName(void) const override
    {
        return ("Controller");
    }

    const char *getApplicationVersion(void) const override
    {
        return ("2.2.0");
    }

    const char *getApplicationSandbox(void) const override
    {
        return ("ctrlv2");
    }

    void printBounds(const Rectangle &bounds)
    {
        logMessage("bounds: [%d %d %d %d]",
                   bounds.getX(),
                   bounds.getY(),
                   bounds.getWidth(),
                   bounds.getHeight());
    }

    void printPage(const Page &page)
    {
        logMessage("id: %d", page.getId());
        logMessage("name: %s", page.getName());
        logMessage("hasObjects: %d", page.getHasObjects());
        logMessage("--");
    }

    void printDevice(const Device &device)
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

    void printGroup(const Group &group)
    {
        logMessage("id: %d", group.getId());
        logMessage("label: %s", group.getLabel());
        logMessage("pageId: %d", group.getPageId());
        logMessage("colour: %d", group.getColour());
        printBounds(group.getBounds());
        logMessage("--");
    }

    void printInput(const Input &input)
    {
        logMessage("    potId: %d", input.getPotId());
        logMessage("    valueId: %d", input.getValueId());
        logMessage("    --");
    }

    void printInputs(const std::vector<Input> &inputs)
	{
		logMessage("    --[Inputs]------------------------------------");
		for (const auto &input : inputs) {
			printInput(input);
		}
    }

	void printValue(const Value2 &value)
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

	void printValues(const std::vector<Value2> &values)
	{
		logMessage("    --[Values]------------------------------------");
		for (const auto &value : values) {
			printValue(value);
		}
	}

    void printControl(const Control &control)
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

    void printMessage(const Message &message)
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

    void initialise(void) override
    {
        logMessage("setup completed");
        stringPool.assignStorageDriver(&Hardware::screen);
        bitmapPool.assignStorageDriver(&Hardware::screen);

        if (preset.load("ctrlv2/p000.epr") == true) {
            logMessage("preset loaded");
        } else {
            logMessage("preset load failed");
        }

        logMessage("--[Preset]-------------------------------------");
        logMessage("name: %s", preset.getName());
        logMessage("projectId: %s", preset.getProjectId());
        logMessage("version: %d", preset.getVersion());
        logMessage("isvalid: %d", preset.isValid());

        logMessage("--[Pages]--------------------------------------");
        for (const auto &page : preset.pages) {
            printPage(page);
        }

        logMessage("--[Devices]------------------------------------");
        for (const auto &device : preset.devices) {
            printDevice(device);
        }

        logMessage("--[Overlays]-----------------------------------");

        logMessage("--[Groups]-------------------------------------");
        for (const auto &group : preset.groups) {
            printGroup(group);
        }

        logMessage("--[Controls]-----------------------------------");
        for (const auto &control : preset.controls) {
            printControl(control);
        }

        logMessage("--[end]----------------------------------------");
    }

    class MainWindow : public Window
    {
    public:
        MainWindow()
        {
            setOwnedContent(new MainComponent());
            setVisible(true);
        }
    };

private:
    MainWindow mainWindow;
    Preset preset;
};

// This macro instructs main() routine to launch the app.
START_ELECTRA_APPLICATION(Controller)
