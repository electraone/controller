#include "App.h"
#include "MainComponent.h"

class Controller : public App
{
public:
    HelloWorld()
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
        return ("control");
    }

    void initialise(void) override
    {
        logMessage("setup completed");
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
};

// This macro instructs main() routine to launch the app.
START_ELECTRA_APPLICATION(Controller)
