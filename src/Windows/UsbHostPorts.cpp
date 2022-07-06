#include "UsbHostPorts.h"

UsbHostPorts::UsbHostPorts(UiApi &newUiApi)
    : uiApi(newUiApi), usbHostDevice{ nullptr }
{
    setName("settings");

    for (uint8_t i = 0; i < 2; i++) {
        usbHostDevice[i] = new UsbHostDevice(USBDevices[i].product,
                                             USBDevices[i].manufacturer,
                                             USBDevices[i].serial,
                                             USBDevices[i].midiBus);

        if (usbHostDevice[i]) {
            usbHostDevice[i]->setId(100 + i);

            if (USBDevices[i].midiDevice == NULL) {
                usbHostDevice[i]->setDisabled(true);
            }

            usbHostDevice[i]->onClick = [this, i](uint8_t port) {
                logMessage(
                    "changeUsbHostLambda: setting USB device %d to port %d",
                    i,
                    port);
                usbHostDevice[i]->setPort(port);
                USBDevices[i].midiBus = port;
            };

            addAndMakeVisible(usbHostDevice[i]);
        }
    }

    setBounds(0, 219, 1024, 357);
}

void UsbHostPorts::paint(Graphics &g)
{
    g.fillAll(Colours::black);
    paintTitleBar(g, "USB Host configuration", getWidth(), 0x0002);
}

void UsbHostPorts::resized(void)
{
    for (uint8_t i = 0; i < 2; i++) {
        usbHostDevice[i]->setBounds(166, 80 + (i * 150), 700, 100);
    }
}

void UsbHostPorts::paintTitleBar(Graphics &g,
                                 const char *title,
                                 uint16_t width,
                                 uint32_t colour)
{
    g.setColour(colour);
    g.fillRect(0, 0, width, 25);
    paintIcon(g, 16, 5);
    g.printText(
        40, 8, title, TextStyle::smallTransparent, width, TextAlign::left);
}

void UsbHostPorts::paintIcon(Graphics &g, uint16_t x, uint16_t y)
{
    g.setColour(Colours::white);
    g.drawRect(x, y, 10, 13);
    g.fillRect(x + 5, y + 3, 10, 13);
}
