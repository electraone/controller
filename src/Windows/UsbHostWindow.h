#pragma once

#include "Window.h"
#include "UsbHostPorts.h"
#include "UiDelegate.h"

class UsbHostWindow : public Window
{
private:
    UsbHostWindow(UiDelegate &newDelegate)
        : delegate(newDelegate), content(nullptr)
    {
        content = new UsbHostPorts(newDelegate);

        if (content) {
            setOwnedContent(content);
        }
        setName("UsbHostWindow");
        setVisible(true);
    }

public:
    virtual ~UsbHostWindow() = default;

    // API

    // Events
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

    // Factory function
    static UsbHostWindow *createUsbHostWindow(UiDelegate &newDelegate)
    {
        return new UsbHostWindow(newDelegate);
    }

private:
    UiDelegate &delegate;
    UsbHostPorts *content;
};
