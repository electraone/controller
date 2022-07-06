#pragma once

#include "Window.h"
#include "UsbHostPorts.h"
#include "UiApi.h"

class UsbHostWindow : public Window
{
private:
    UsbHostWindow(UiApi &newUiApi) : uiApi(newUiApi), content(nullptr)
    {
        content = new UsbHostPorts(newUiApi);

        if (content) {
            setOwnedContent(content);
        }
        setName("UsbHostWindow");
        setVisible(true);
        logMessage("done");
    }

public:
    virtual ~UsbHostWindow() = default;

    // API

    // Events
    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            uiApi.usbHost_close();
        } else if (buttonId == 4) {
            uiApi.usbHost_openPresetSelection();
        } else if (buttonId == 5) {
            uiApi.usbHost_openPageSelection();
        }
    }

    // Factory function
    static UsbHostWindow *createUsbHostWindow(UiApi &newUiApi)
    {
        return new UsbHostWindow(newUiApi);
    }

private:
    UiApi &uiApi;
    UsbHostPorts *content;
};
