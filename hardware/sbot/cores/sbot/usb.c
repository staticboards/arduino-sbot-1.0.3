#define USB_SERIAL

#if defined(USB_SERIAL)
#include "../usb_serial/usb.c"
#elif defined(USB_HID)
#include "../usb_hid/usb.c"
#elif defined(USB_SERIAL_HID)
#include "../usb_serial_hid/usb.c"
#elif defined(USB_DISK) || defined(USB_DISK_SDFLASH)
#include "../usb_disk/usb.c"
#elif defined(USB_MIDI)
#include "../usb_midi/usb.c"
#elif defined(USB_RAWHID)
#include "../usb_rawhid/usb.c"
#elif defined(USB_FLIGHTSIM)
#include "../usb_flightsim/usb.c"
#endif
