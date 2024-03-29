/* USB Serial Example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_serial.html
 * Copyright (c) 2008 PJRC.COM, LLC
 *
 * modified by staticboards for the sbot controller
 * http://staticboards.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "usb_common.h"
#include "usb_private.h"



/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/


static const uint8_t PROGMEM endpoint_config_table[] = {
	1, EP_TYPE_INTERRUPT_IN,  EP_SIZE(DEBUG_TX_SIZE) | DEBUG_TX_BUFFER,
	1, EP_TYPE_INTERRUPT_OUT, EP_SIZE(DEBUG_RX_SIZE) | DEBUG_RX_BUFFER,
	1, EP_TYPE_INTERRUPT_IN,  EP_SIZE(FLIGHTSIM_TX_SIZE) | FLIGHTSIM_TX_BUFFER,
	1, EP_TYPE_INTERRUPT_OUT, EP_SIZE(FLIGHTSIM_RX_SIZE) | FLIGHTSIM_RX_BUFFER,
	0
};


/**************************************************************************
 *
 *  Descriptor Data
 *
 **************************************************************************/

// Descriptors are the data that your computer reads when it auto-detects
// this USB device (called "enumeration" in USB lingo).  The most commonly
// changed items are editable at the top of this file.  Changing things
// in here should only be done by those who've read chapter 9 of the USB
// spec and relevant portions of any USB class specifications!

static const uint8_t PROGMEM device_descriptor[] = {
	18,					// bLength
	1,					// bDescriptorType
	0x00, 0x02,				// bcdUSB
	0,					// bDeviceClass
	0,					// bDeviceSubClass
	0,					// bDeviceProtocol
	ENDPOINT0_SIZE,				// bMaxPacketSize0
	LSB(VENDOR_ID), MSB(VENDOR_ID),		// idVendor
	LSB(PRODUCT_ID), MSB(PRODUCT_ID),	// idProduct
	0x02, 0x01,				// bcdDevice
	0,					// iManufacturer
	1,					// iProduct
	0,					// iSerialNumber
	1					// bNumConfigurations
};


static uint8_t PROGMEM rawhid_hid_report_desc[] = {
        0x06, 0x1C, 0xFF,			// Usage page = 0xFF1C
        0x0A, 0x39, 0xA7,			// Usage = 0xA739
        0xA1, 0x01,                             // Collection 0x01
        0x75, 0x08,                             // report size = 8 bits
        0x15, 0x00,                             // logical minimum = 0
        0x26, 0xFF, 0x00,                       // logical maximum = 255
        0x95, FLIGHTSIM_TX_SIZE,                   // report count
        0x09, 0x01,                             // usage
        0x81, 0x02,                             // Input (array)
        0x95, FLIGHTSIM_RX_SIZE,                   // report count
        0x09, 0x02,                             // usage
        0x91, 0x02,                             // Output (array)
        0xC0                                    // end collection
};

static const uint8_t PROGMEM debug_hid_report_desc[] = {
        0x06, 0xC9, 0xFF,                       // Usage Page 0xFFC9 (vendor defined)
        0x09, 0x04,                             // Usage 0x04
        0xA1, 0x5C,                             // Collection 0x5C
        0x75, 0x08,                             // report size = 8 bits (global)
        0x15, 0x00,                             // logical minimum = 0 (global)
        0x26, 0xFF, 0x00,                       // logical maximum = 255 (global)
        0x95, DEBUG_TX_SIZE,                    // report count (global)
        0x09, 0x75,                             // usage (local)
        0x81, 0x02,                             // Input
        0x95, DEBUG_RX_SIZE,                    // report count (global)
        0x09, 0x76,                             // usage (local)
        0x91, 0x02,                             // Output
        0x95, 0x04,                             // report count (global)
        0x09, 0x76,                             // usage (local)
        0xB1, 0x02,                             // Feature
        0xC0                                    // end collection
};


#define FLIGHTSIM_HID_DESC_OFFSET	( 9 + 9 )
#define DEBUG_HID_DESC_OFFSET		( 9 + 9+9+7+7 + 9 )
#define CONFIG1_DESC_SIZE		( 9 + 9+9+7+7 + 9+9+7+7 )

static const uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9, 					// bLength;
	2,					// bDescriptorType;
	LSB(CONFIG1_DESC_SIZE),			// wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	NUM_INTERFACE,				// bNumInterfaces
	1,					// bConfigurationValue
	0,					// iConfiguration
	0xC0,					// bmAttributes
	50,					// bMaxPower

        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        FLIGHTSIM_INTERFACE,                       // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass (0x01 = Boot)
        0x00,                                   // bInterfaceProtocol (0x01 = Keyboard)
        2,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        sizeof(rawhid_hid_report_desc),         // wDescriptorLength
        0,
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        FLIGHTSIM_TX_ENDPOINT | 0x80,           // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        FLIGHTSIM_TX_SIZE, 0,                   // wMaxPacketSize
        FLIGHTSIM_TX_INTERVAL,                  // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        FLIGHTSIM_RX_ENDPOINT,                  // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        FLIGHTSIM_RX_SIZE, 0,			// wMaxPacketSize
        FLIGHTSIM_RX_INTERVAL,                  // bInterval

        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        DEBUG_INTERFACE,                        // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        3,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        sizeof(debug_hid_report_desc),          // wDescriptorLength
        0,
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        DEBUG_TX_ENDPOINT | 0x80,               // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        DEBUG_TX_SIZE, 0,                       // wMaxPacketSize
        DEBUG_TX_INTERVAL,                      // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        DEBUG_RX_ENDPOINT,                      // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        DEBUG_RX_SIZE, 0,                       // wMaxPacketSize
        DEBUG_RX_INTERVAL                       // bInterval

};

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	int16_t wString[];
};
static const struct usb_string_descriptor_struct PROGMEM string0 = {
	4,
	3,
	{0x0409}
};
static const struct usb_string_descriptor_struct PROGMEM string1 = {
	sizeof(STR_PRODUCT),
	3,
	STR_PRODUCT
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const struct descriptor_list_struct {
	uint16_t	wValue;
	uint16_t	wIndex;
	const uint8_t	*addr;
	uint8_t		length;
} PROGMEM descriptor_list[] = {
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
	{0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
        {0x2200, FLIGHTSIM_INTERFACE, rawhid_hid_report_desc, sizeof(rawhid_hid_report_desc)},
        {0x2100, FLIGHTSIM_INTERFACE, config1_descriptor+FLIGHTSIM_HID_DESC_OFFSET, 9},
        {0x2200, DEBUG_INTERFACE, debug_hid_report_desc, sizeof(debug_hid_report_desc)},
        {0x2100, DEBUG_INTERFACE, config1_descriptor+DEBUG_HID_DESC_OFFSET, 9},
	{0x0300, 0x0000, (const uint8_t *)&string0, 4},
	{0x0301, 0x0409, (const uint8_t *)&string1, sizeof(STR_PRODUCT)},
};
#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))


/**************************************************************************
 *
 *  Variables - these are the only non-stack RAM usage
 *
 **************************************************************************/

// zero when we are not configured, non-zero when enumerated
volatile uint8_t usb_configuration USBSTATE;
volatile uint8_t usb_suspended USBSTATE;

// the time remaining before we transmit any partially full
// packet, or send a zero length packet.
volatile uint8_t debug_flush_timer USBSTATE;

// these are a more reliable timeout than polling the
// frame counter (UDFNUML)
volatile uint16_t rawhid_rx_timeout_count USBSTATE;
volatile uint16_t rawhid_tx_timeout_count USBSTATE;



/**************************************************************************
 *
 *  Public Functions - these are the API intended for the user
 *
 **************************************************************************/



// initialize USB serial
void usb_init(void)
{
	uint8_t u;

	u = USBCON;
	if ((u & (1<<USBE)) && !(u & (1<<FRZCLK))) return;
	HW_CONFIG();
        USB_FREEZE();				// enable USB
        PLL_CONFIG();				// config PLL
        while (!(PLLCSR & (1<<PLOCK))) ;	// wait for PLL lock
        USB_CONFIG();				// start USB clock
        UDCON = 0;				// enable attach resistor
	usb_configuration = 0;
	usb_suspended = 0;
	debug_flush_timer = 0;
	rawhid_rx_timeout_count = 0;
	rawhid_tx_timeout_count = 0;
	UDINT = 0;
        UDIEN = (1<<EORSTE)|(1<<SOFE);
	//sei();  // init() in wiring.c does this
}

void usb_shutdown(void)
{
	UDIEN = 0;		// disable interrupts
	UDCON = 1;		// disconnect attach resistor
	USBCON = 0;		// shut off USB periperal
	PLLCSR = 0;		// shut off PLL
	usb_configuration = 0;
	usb_suspended = 1;
}


// Public API functions moved to usb_api.cpp

/**************************************************************************
 *
 *  Private Functions - not intended for general user consumption....
 *
 **************************************************************************/


// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{
	uint8_t intbits, t, ep;

        intbits = UDINT;
        UDINT = 0;
	ep = UENUM;
        if (intbits & (1<<EORSTI)) {
		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
		UEIENX = (1<<RXSTPE);
		usb_configuration = 0;
        }
        if ((intbits & (1<<SOFI)) && usb_configuration) {
		UENUM = FLIGHTSIM_TX_ENDPOINT;
		t = UEBCLX;
		if (t > 0) {
			for (; t < FLIGHTSIM_TX_SIZE; t++) {
				UEDATX = 0;
			}
			UEINTX = 0x3A;
		}
                t = debug_flush_timer;
                if (t) {
                        debug_flush_timer = -- t;
                        if (!t) {
                                UENUM = DEBUG_TX_ENDPOINT;
                                while ((UEINTX & (1<<RWAL))) {
                                        UEDATX = 0;
                                }
                                UEINTX = 0x3A;
                        }
                }
        }
	if (intbits & (1<<SUSPI)) {
		// USB Suspend (inactivity for 3ms)
		UDIEN = (1<<WAKEUPE);
		usb_configuration = 0;
		usb_suspended = 1;
		#if (F_CPU >= 8000000L)
		// WAKEUPI does not work with USB clock freeze 
		// when CPU is running less than 8 MHz.
		// Is this a hardware bug?
		USB_FREEZE();			// shut off USB
		PLLCSR = 0;			// shut off PLL
		#endif
		// to properly meet the USB spec, current must
		// reduce to less than 2.5 mA, which means using
		// powerdown mode, but that breaks the Arduino
		// user's paradigm....
	}
	if (usb_suspended && (intbits & (1<<WAKEUPI))) {
		// USB Resume (pretty much any activity)
		#if (F_CPU >= 8000000L)
		PLL_CONFIG();
		while (!(PLLCSR & (1<<PLOCK))) ;
		USB_CONFIG();
		#endif
		UDIEN = (1<<EORSTE)|(1<<SOFE)|(1<<SUSPE);
		usb_suspended = 0;
		return;
	}
	UENUM = ep;
}


// Misc functions to wait for ready and send/receive packets
static inline void usb_wait_in_ready(void)
{
	while (!(UEINTX & (1<<TXINI))) ;
}
static inline void usb_send_in(void)
{
	UEINTX = ~(1<<TXINI);
}
static inline void usb_wait_receive_out(void)
{
	while (!(UEINTX & (1<<RXOUTI))) ;
}
static inline void usb_ack_out(void)
{
	UEINTX = ~(1<<RXOUTI);
}



// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR(USB_COM_vect)
{
        uint8_t intbits;
	const uint8_t *list;
        const uint8_t *cfg;
	uint8_t i, n, len, en;
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t desc_val;
	const uint8_t *desc_addr;
	uint8_t	desc_length;

	UENUM = 0;
	intbits = UEINTX;
	if (intbits & (1<<RXSTPI)) {
		bmRequestType = UEDATX;
		bRequest = UEDATX;
		read_word_lsbfirst(wValue, UEDATX);
		read_word_lsbfirst(wIndex, UEDATX);
		read_word_lsbfirst(wLength, UEDATX);
		UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
		if (bRequest == GET_DESCRIPTOR) {
			list = (const uint8_t *)descriptor_list;
			for (i=0; ; i++) {
				if (i >= NUM_DESC_LIST) {
					UECONX = (1<<STALLRQ)|(1<<EPEN);  //stall
					return;
				}
				pgm_read_word_postinc(desc_val, list);
				if (desc_val != wValue) {
					list += sizeof(struct descriptor_list_struct)-2;
					continue;
				}
				pgm_read_word_postinc(desc_val, list);
				if (desc_val != wIndex) {
					list += sizeof(struct descriptor_list_struct)-4;
					continue;
				}
				pgm_read_word_postinc(desc_addr, list);
				desc_length = pgm_read_byte(list);
				break;
			}
			len = (wLength < 256) ? wLength : 255;
			if (len > desc_length) len = desc_length;
			list = desc_addr;
			do {
				// wait for host ready for IN packet
				do {
					i = UEINTX;
				} while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
				if (i & (1<<RXOUTI)) return;	// abort
				// send IN packet
				n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
				for (i = n; i; i--) {
					pgm_read_byte_postinc(UEDATX, list);
				}
				len -= n;
				usb_send_in();
			} while (len || n == ENDPOINT0_SIZE);
			return;
                }
		if (bRequest == SET_ADDRESS) {
			usb_send_in();
			usb_wait_in_ready();
			UDADDR = wValue | (1<<ADDEN);
			return;
		}
		if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
			usb_configuration = wValue;
			debug_flush_timer = 0;
			usb_send_in();
			cfg = endpoint_config_table;
			for (i=1; i<NUM_ENDPOINTS; i++) {
				UENUM = i;
				pgm_read_byte_postinc(en, cfg);
				UECONX = en;
				if (en) {
					pgm_read_byte_postinc(UECFG0X, cfg);
					pgm_read_byte_postinc(UECFG1X, cfg);
				}
			}
        		UERST = 0x1E;
        		UERST = 0;
			return;
		}
		if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
			usb_wait_in_ready();
			UEDATX = usb_configuration;
			usb_send_in();
			return;
		}
		if (bRequest == GET_STATUS) {
			usb_wait_in_ready();
			i = 0;
			if (bmRequestType == 0x82) {
				UENUM = wIndex;
				if (UECONX & (1<<STALLRQ)) i = 1;
				UENUM = 0;
			}
			UEDATX = i;
			UEDATX = 0;
			usb_send_in();
			return;
		}
		if ((bRequest == CLEAR_FEATURE || bRequest == SET_FEATURE)
		  && bmRequestType == 0x02 && wValue == 0) {
			i = wIndex & 0x7F;
			if (i >= 1 && i <= NUM_ENDPOINTS) {
				usb_send_in();
				UENUM = i;
				if (bRequest == SET_FEATURE) {
					UECONX = (1<<STALLRQ)|(1<<EPEN);
				} else {
					UECONX = (1<<STALLRQC)|(1<<RSTDT)|(1<<EPEN);
					UERST = (1 << i);
					UERST = 0;
				}
				return;
			}
		}
                if (wIndex == FLIGHTSIM_INTERFACE) {
                        if (bmRequestType == 0xA1 && bRequest == HID_GET_REPORT) {
                                len = FLIGHTSIM_TX_SIZE;
                                do {
                                        // wait for host ready for IN packet
                                        do {
                                                i = UEINTX;
                                        } while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
                                        if (i & (1<<RXOUTI)) return;    // abort
                                        // send IN packet
                                        n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
                                        for (i = n; i; i--) {
                                                // just send zeros
                                                UEDATX = 0;
                                        }
                                        len -= n;
                                        usb_send_in();
                                } while (len || n == ENDPOINT0_SIZE);
                                return;
                        }
                        if (bmRequestType == 0x21 && bRequest == HID_SET_REPORT) {
                                len = FLIGHTSIM_RX_SIZE;
                                do {
                                        n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
                                        usb_wait_receive_out();
                                        // ignore incoming bytes
                                        usb_ack_out();
                                        len -= n;
                                } while (len);
                                usb_wait_in_ready();
                                usb_send_in();
                                return;
                        }
                }
                if (wIndex == DEBUG_INTERFACE) {
                        if (bRequest == HID_GET_REPORT && bmRequestType == 0xA1) {
                                len = wLength;
                                do {
                                        // wait for host ready for IN packet
                                        do {
                                                i = UEINTX;
                                        } while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
                                        if (i & (1<<RXOUTI)) return;    // abort
                                        // send IN packet
                                        n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
                                        for (i = n; i; i--) {
                                                UEDATX = 0;
                                        }
                                        len -= n;
                                        usb_send_in();
                                } while (len || n == ENDPOINT0_SIZE);
                                return;
                        }
                        if (bRequest == HID_SET_REPORT && bmRequestType == 0x21) {
				if (wValue == 0x0300 && wLength == 0x0004) {
					uint8_t b1, b2, b3, b4;
                                        usb_wait_receive_out();
					b1 = UEDATX;
					b2 = UEDATX;
					b3 = UEDATX;
					b4 = UEDATX;
                                        usb_ack_out();
                                        usb_send_in();
					if (b1 == 0xA9 && b2 == 0x45 && b3 == 0xC2 && b4 == 0x6B)
						_reboot_sbot_();
					if (b1 == 0x8B && b2 == 0xC5 && b3 == 0x1D && b4 == 0x70)
						_restart_sbot_();
				}
			}
                }
		if (bRequest == 0xC9 && bmRequestType == 0x40) {
			usb_send_in();
			usb_wait_in_ready();
			_restart_sbot_();
		}
        }
	UECONX = (1<<STALLRQ) | (1<<EPEN);	// stall
}


