//*****************************************************************************
//
// usb_dev_gamepad.c - Main routines for the gamepad example.Z
// Copyright (c) Aditya Challamarad.  All rights reserved.
// 
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidgamepad.h"
#include "usb_gamepad_structs.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"


static tGamepadReport sReport; // The report structure that is passed to the HID gamepad driver.


static uint32_t g_pui32ADCData[3]; // The HID gamepad polled ADC data for the X/Y/Z coordinates. X = Joystick X, Y = Joystick Y, Z = Potentiometer

static uint32_t g_ui32Updates; // An activity counter to slow the LED blink down to a visible rate.


volatile enum // holds the various states of the gamepad. volatile as it is changed in interrupt.
{
    // Not yet configured.
    eStateNotConfigured,

    // Connected and not waiting on data to be sent.
    eStateIdle,

    // Suspended.
    eStateSuspend,

    // Connected and waiting on data to be sent out.
    eStateSending
} g_iGamepadState;


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


// This maps the values from the ADC that range from 0 to 2047 over to 127 to -128.
#define Convert8Bit(ui32Value)  ((int8_t)((0x7ff - ui32Value) >> 4))



// Handles asynchronous events from the HID gamepad driver.
//
// pvCBData is the event callback pointer provided during USBDHIDGamepadInit().
// This is a pointer to our gamepad device structure
// (&g_sGamepadDevice).

// ui32Event identifies the event we are being called back for.

// ui32MsgData is an event-specific value.

// pvMsgData is an event-specific pointer.

// This function is called by the HID gamepad driver to inform the application
// of particular asynchronous events related to operation of the gamepad HID
// device.
//
// \return Returns 0 in all cases.

uint32_t GamepadHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgData,
               void *pvMsgData)
{
    switch (ui32Event)
    {
        // host connected
        case USB_EVENT_CONNECTED:
        {
            g_iGamepadState = eStateIdle;

            UARTprintf("\nHost Connected...\n");

            break;
        }

        // host disconnected
        case USB_EVENT_DISCONNECTED:
        {
            g_iGamepadState = eStateNotConfigured;


            UARTprintf("\nHost Disconnected...\n");

            break;
        }

        // This event indicates that a report we sent has completed.

        case USB_EVENT_TX_COMPLETE:
        {
            
            g_iGamepadState = eStateIdle; // enter idle state

            MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);

            break;
        }

        // suspend event
        case USB_EVENT_SUSPEND:
        {
     
            g_iGamepadState = eStateSuspend; // enter suspend state

            UARTprintf("\nBus Suspended\n");

            MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);

            break;
        }

    
        // This event signals that the host has resumed signaling on the bus.

        case USB_EVENT_RESUME:
        {
            // Return to the idle state.
            g_iGamepadState = eStateIdle;

    
            UARTprintf("\nBus Resume\n");

            break;
        }

        // Return the pointer to the current report.  This call is
        // rarely made but is required by the USB HID spec.
        case USBD_HID_EVENT_GET_REPORT:
        {
            *(void **)pvMsgData = (void *)&sReport;
            break;
        }

        // ignore everything else
        default:
        {
            break;
        }
    }

    return(0);
}

// uart config
void ConfigureUART(void)
{
    // enable PORT A used for UART0
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // enable UART0
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);

    // Select the alternate (UART) function for these pins.
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Useinternal 16MHz clock for UART.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART at 115200 baud.
    UARTStdioConfig(0, 115200, 16000000);
}

// Initialize the ADC inputs used by the game pad device. GamePad uses E: 1,4,5 (AIN: 2,9,8)
void ADCInit(void)
{
    // enable GPIO port E for ADC inputs
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // enable port Potentiometer and Joystick VRx/VRy

    
    // use AHB for GPIO
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOE); // high performance bus

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);

    // select external ref for ADC
    MAP_ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);

    // Configure the pins which are used as analog inputs.
    MAP_GPIOPinTypeADC(GPIO_PORTE_AHB_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_1);

    // Configure the sequencer for 3 steps.
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH8);

    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH9);

    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
    

    // Enable the sequence but do not start it yet.
    MAP_ADCSequenceEnable(ADC0_BASE, 0);
}


int main(void) // this runs the main code
{
    uint8_t ui8ButtonsChanged, ui8Buttons; // button state variables
    bool bUpdate;

    // Set the clocking to run from the PLL at 50MHz
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    // Enable the GPIO port that is used for the on-board LED.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Enable the GPIO pin for the RED LED (PF1).
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

    // UART0 config
    ConfigureUART();

    UARTprintf("\033[2JGAMEPAD\n"); // clear screen then print
    UARTprintf("---------------------------------\n\n");

    // Not configured initially
    g_iGamepadState = eStateNotConfigured;

    // Enable the GPIO peripheral used for USB, and configure the USB
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD);
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_AHB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // init function for buttons
    ButtonsInit();

    // Initialize the ADC channels.
    ADCInit();

    UARTprintf("Configuring USB\n");

    // Set the USB stack mode to Device mode.
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    // Initialize the HID gamepad device.
    USBDHIDGamepadInit(0, &g_sGamepadDevice);

    // Zero out the initial report
    sReport.ui8Buttons = 0;
    sReport.i8XPos = 0;
    sReport.i8YPos = 0;
    sReport.i8LT = 0;
    sReport.i8RT = 0;

    UARTprintf("\nWaiting For Host...\n");

    IntMasterEnable(); // enale interrupts
    // Trigger ADC sample
    ADCProcessorTrigger(ADC0_BASE, 0);

    while(1)
    {
        // wait till connected
        if(g_iGamepadState == eStateIdle)
        {
            
            bUpdate = false;

            // poll buttons to see if clicked
            ButtonsPoll(&ui8ButtonsChanged, &ui8Buttons);
            ui8Buttons ^= JOYSTICK_MASK;  // Invert the joystick switch bit as it is active low

            sReport.ui8Buttons = 0; // initially reset buttons


            if(ui8Buttons & BUTTON1_MASK) //  if button 1 clicked
            {
                sReport.ui8Buttons |= BUTTON1_MASK; // set button 1 bit
            }

            if(ui8Buttons & BUTTON2_MASK) // if button 2 clicked
            {
                sReport.ui8Buttons |= BUTTON2_MASK; // set button 2 bit
            }

            if(ui8Buttons & BUTTON3_MASK) // if button 3 clicked
            {
                sReport.ui8Buttons |= BUTTON3_MASK; // set button 3 bit
            }

            if(ui8Buttons & BUTTON4_MASK) // if button 4 clicked
            {
                sReport.ui8Buttons |= BUTTON4_MASK; //  set button 4 bit
            }

            if(ui8Buttons & JOYSTICK_MASK) // if joystick clicked
            {
                sReport.ui8Buttons |= JOYSTICK_MASK; // set joystick button bit
            }


            // check if ADC done converting
            if(ADCIntStatus(ADC0_BASE, 0, false) != 0)
            {
                // clear ADC int
                ADCIntClear(ADC0_BASE, 0);

                // read data and trigger new sample
                ADCSequenceDataGet(ADC0_BASE, 0, &g_pui32ADCData[0]);
                ADCProcessorTrigger(ADC0_BASE, 0);

                // update the report with ADC data
                sReport.i8XPos = Convert8Bit(g_pui32ADCData[0]);
                sReport.i8YPos = Convert8Bit(g_pui32ADCData[2]);
                int32_t pot = g_pui32ADCData[1]; // 0 to 4095 

                if (pot < 2048) { // if pot less than middle, left is triggered 
                    sReport.i8LT = (uint8_t)((2048 - pot) * 255 / 2048);   // trigger strength in LT
                    sReport.i8RT = 0;      

                } else if (pot > 2048) { // if pot more than middle, right is triggered
                    sReport.i8LT = 0;  
                    sReport.i8RT = (uint8_t)((pot - 2048) * 255 / 2047);    // trigger strength in RT

                } else {
                    sReport.i8LT = 0;
                    sReport.i8RT = 0;
                }
                bUpdate = true; 
            }

            // send report if updated values.
            if(bUpdate)
            {
                USBDHIDGamepadSendReport(&g_sGamepadDevice, &sReport,
                                         sizeof(sReport));

                // change state to sending 
                IntMasterDisable(); // disable interrupts because changing state
                g_iGamepadState = eStateSending; 
                IntMasterEnable(); // enable interrupts

                // Limit the blink rate of the LED.
                if(g_ui32Updates++ == 40) // slow down LED blink
                {

                    // Turn on the RED LED.
                    MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

                    g_ui32Updates = 0;
                }
            }
        }
    }
}
