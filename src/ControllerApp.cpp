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

    void initialise(void) override
    {
        logMessage("setup completed");
		stringPool.assignStorageDriver(&Hardware::screen);
        bitmapPool.assignStorageDriver(&Hardware::screen);

		if (preset.load ("ctrlv2/p000.epr") == true)
        {
            logMessage("preset loaded");
		}
		else {
			logMessage("preset load failed");
		}
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
