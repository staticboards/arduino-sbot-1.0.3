#ifndef CORE_SBOT
#define CORE_SBOT

#define USB_SERIAL

#if defined(USB_SERIAL)
#include "../usb_serial/core_id.h"
#elif defined(USB_HID)
#include "../usb_hid/core_id.h"
#elif defined(USB_SERIAL_HID)
#include "../usb_serial_hid/core_id.h"
#elif defined(USB_DISK) || defined(USB_DISK_SDFLASH)
#include "../usb_disk/core_id.h"
#elif defined(USB_MIDI)
#include "../usb_midi/core_id.h"
#elif defined(USB_RAWHID)
#include "../usb_rawhid/core_id.h"
#elif defined(USB_FLIGHTSIM)
#include "../usb_flightsim/core_id.h"
#endif
#endif
