#pragma once

#include "Component.h"
#include "UsbHostDevice.h"
#include "UiApi.h"
#include "USBDevice.h"

extern USBDevice USBDevices[];

class UsbHostPorts : public Component
{
public:
    UsbHostPorts(UiApi &newUiApi);
    virtual ~UsbHostPorts() = default;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    static void paintTitleBar(Graphics &g,
                              const char *title,
                              uint16_t width,
                              uint32_t colour);
    static void paintIcon(Graphics &g, uint16_t x, uint16_t y);

    UiApi &uiApi;

    UsbHostDevice *usbHostDevice[2];
    static constexpr uint8_t topPadding = 50;
};
