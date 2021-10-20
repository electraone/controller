#include "App.h"
#include "MainComponent.h"
#include "Preset.h"

#include "MemoryPool.h"
#include "BitmapPool.h"

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
        for (const auto &[id, page] : preset.pages) {
            printPage(page);
        }

        logMessage("--[Devices]------------------------------------");
        for (const auto &[id, device] : preset.devices) {
            printDevice(device);
        }

        logMessage("--[Overlays]-----------------------------------");
        for (const auto &[id, overlay] : preset.overlays) {
            printOverlay(overlay);
        }

        logMessage("--[Groups]-------------------------------------");
        for (const auto &[id, group] : preset.groups) {
            printGroup(group);
        }

        logMessage("--[Controls]-----------------------------------");
        for (const auto &[id, control] : preset.controls) {
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
    void printBounds(const Rectangle &bounds);
    void printPage(const Page &page);
    void printDevice(const Device &device);
    void printOverlay(const Overlay &overlay);
    void printGroup(const Group &group);
    void printInput(const Input &input);
    void printInputs(const std::vector<Input> &inputs);
    void printValue(const Value2 &value);
    void printValues(const std::vector<Value2> &values);
    void printControl(const Control &control);
    void printMessage(const Message &message);

    MainWindow mainWindow;
    Preset preset;
};

// This macro instructs main() routine to launch the app.
START_ELECTRA_APPLICATION(Controller)
