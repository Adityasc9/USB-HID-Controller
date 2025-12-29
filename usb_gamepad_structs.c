#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidgamepad.h"
#include "usb_gamepad_structs.h"

const uint8_t g_pui8LangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

// The manufacturer string (me)
const uint8_t g_pui8ManufacturerString[] =
{
    (6 + 1) * 2,
    USB_DTYPE_STRING,
    'A',0,'d',0,'i',0,'t',0,'y',0,'a',0
};

// The product. (RL Gamepad)
const uint8_t g_pui8ProductString[] =
{
    (10 + 1) * 2,
    USB_DTYPE_STRING,
    'R',0,'L',0,' ',0,'G',0,'a',0,'m',0,'e',0,'p',0,'a',0,'d',0
};


// The serial number, doesnt matter so 12345678.

const uint8_t g_pui8SerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};


// The interface description string.
const uint8_t g_pui8HIDInterfaceString[] =
{
    (21 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'G', 0, 'a', 0, 'm', 0, 'e', 0,
    'p', 0, 'a', 0, 'd', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0,
    'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

// The configuration description string.
const uint8_t g_pui8ConfigString[] =
{
    (25 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'G', 0, 'a', 0, 'm', 0, 'e', 0,
    'p', 0, 'a', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0,
    'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0,
    'n', 0
};

// The descriptor string table.
const uint8_t * const g_ppui8StringDescriptors[] =
{
    g_pui8LangDescriptor,
    g_pui8ManufacturerString,
    g_pui8ProductString,
    g_pui8SerialNumberString,
    g_pui8HIDInterfaceString,
    g_pui8ConfigString
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_ppui8StringDescriptors) / sizeof(uint8_t *))

// The HID game pad device initialization and customization structures.

uint8_t g_pui8MyCustomReportDescriptor[] =
{
    UsagePage(USB_HID_GENERIC_DESKTOP),
    Usage(USB_HID_GAME_PAD),
    Collection(USB_HID_APPLICATION),
        // joystick
        UsagePage(USB_HID_GENERIC_DESKTOP),
        Usage(USB_HID_X), // X axis
        Usage(USB_HID_Y), // Y axis

        LogicalMinimum(-128),
        LogicalMaximum(127),
        ReportSize(8),
        ReportCount(2),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),

        // potentiometer
        Usage(USB_HID_Z),// LT (trigger left) left potentiometer
        Usage(USB_HID_RZ), // RT (trigger right) right potentiometer
        LogicalMinimum(0),
        LogicalMaximum(255),
        ReportSize(8),
        ReportCount(2),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),

        // buttons
        UsagePage(USB_HID_BUTTONS),
        UsageMinimum(1),
        UsageMaximum(5), // 5 buttons (1 joystick + 4 regular)
        LogicalMinimum(0),
        LogicalMaximum(1),
        ReportSize(1),
        ReportCount(5),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),

        // 3 bits padding to fill out a byte, alignment
        ReportSize(1),
        ReportCount(3),
        Input(USB_HID_INPUT_CONSTANT | USB_HID_INPUT_ARRAY | USB_HID_INPUT_ABS),

    EndCollection
};


tUSBDHIDGamepadDevice g_sGamepadDevice = {// gamepad device structure {
    USB_VID_TI_1CBE,
    USB_PID_GAMEPAD,
    0,
    USB_CONF_ATTR_SELF_PWR,
    GamepadHandler,
    (void *)&g_sGamepadDevice,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS,
    g_pui8MyCustomReportDescriptor,
    sizeof(g_pui8MyCustomReportDescriptor)
};
