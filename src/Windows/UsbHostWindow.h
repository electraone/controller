#pragma once

#include "Window.h"
#include "UsbHostPorts.h"
#include "Model/Page.h"
#include "UiDelegate.h"

class UsbHostWindow : public Window
{
private:
    UsbHostWindow(UiDelegate &newDelegate)
        : delegate(newDelegate), settings(nullptr)
    {
        UsbHostPorts *settings = new UsbHostPorts(newDelegate);

        if (settings) {
            setOwnedContent(settings);
            setVisible(true);
        }

        setName("UsbHostWindow");
    }

public:
    virtual ~UsbHostWindow() = default;

    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            delegate.closeUsbHostPorts();
        } else if (buttonId == 4) {
            delegate.closeUsbHostPorts();
            delegate.openPresetSelection();
        } else if (buttonId == 5) {
            delegate.closeUsbHostPorts();
            delegate.openPageSelection();
        }
    }

    static UsbHostWindow *createUsbHostWindow(UiDelegate &newDelegate)
    {
        UsbHostWindow *sw = new UsbHostWindow(newDelegate);

        if (sw) {
            sw->repaint();
        }

        return (sw);
    }

private:
    UiDelegate &delegate;
    UsbHostPorts *settings;
};
