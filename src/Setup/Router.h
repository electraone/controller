#pragma once

#include <stdint.h>

struct Router {
    const static uint8_t notSet = 255;

    Router()
        : usbDevToMidiIo(true),
          usbDevToUsbHost(true),
          midiIoToUsbDev(true),
          midiIoToUsbHost(true),
          usbHostToMidiIo(true),
          usbHostToUsbDev(true),
          midiIo1Thru(false),
          midiIo2Thru(false),
          usbDevCtrl(notSet),
          midiControlChannel(notSet)
    {
    }

    bool usbDevToMidiIo;
    bool usbDevToUsbHost;
    bool midiIoToUsbDev;
    bool midiIoToUsbHost;
    bool usbHostToMidiIo;
    bool usbHostToUsbDev;
    bool midiIo1Thru;
    bool midiIo2Thru;
    uint8_t usbDevCtrl;
    uint8_t midiControlChannel;
};
