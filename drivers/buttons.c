//*****************************************************************************
//
// buttons.c - Evaluation board driver for push buttons.
//
// Copyright (c) 2012-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "drivers/buttons.h"

//*****************************************************************************
//
//! \addtogroup buttons_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Holds the current, debounced state of each button.  A 0 in a bit indicates
// that that button is currently pressed, otherwise it is released.
// We assume that we start with all the buttons released (though if one is
// pressed when the application starts, this will be detected).
//
//*****************************************************************************
static uint8_t g_ui8ButtonStates = ALL_BUTTONS;


//*****************************************************************************
//
//! Polls the current state of the buttons and determines which have changed.
//!
//! \param pui8Delta points to a character that will be written to indicate
//! which button states changed since the last time this function was called.
//! This value is derived from the debounced state of the buttons.
//! \param pui8RawState points to a location where the raw button state will
//! be stored.
//!
//! This function should be called periodically by the application to poll the
//! pushbuttons.  It determines both the current debounced state of the buttons
//! and also which buttons have changed state since the last time the function
//! was called.
//!
//! In order for button debouncing to work properly, this function should be
//! caled at a regular interval, even if the state of the buttons is not needed
//! that often.
//!
//! If button debouncing is not required, the the caller can pass a pointer
//! for the \e pui8RawState parameter in order to get the raw state of the
//! buttons.  The value returned in \e pui8RawState will be a bit mask where
//! a 1 indicates the buttons is pressed.
//!
//! \return Returns the current debounced state of the buttons where a 1 in the
//! button ID's position indicates that the button is pressed and a 0
//! indicates that it is released.
//
//*****************************************************************************
uint8_t
ButtonsPoll(uint8_t *pui8Delta, uint8_t *pui8RawState)
{
    uint32_t ui32Delta;
    uint32_t ui32Data;
    static uint8_t ui8SwitchClockA = 0;
    static uint8_t ui8SwitchClockB = 0;

    //
    // Read the raw state of the push buttons.  Save the raw state
    // (inverting the bit sense) if the caller supplied storage for the
    // raw value.
    //

    ui32Data = 0;

    if (MAP_GPIOPinRead(BUTTON1_GPIO_BASE, BUTTON1_PIN) & BUTTON1_PIN) ui32Data |= BUTTON1_MASK; //button 1
    if (MAP_GPIOPinRead(BUTTON2_GPIO_BASE, BUTTON2_PIN) & BUTTON2_PIN) ui32Data |= BUTTON2_MASK;
    if (MAP_GPIOPinRead(BUTTON3_GPIO_BASE, BUTTON3_PIN) & BUTTON3_PIN) ui32Data |= BUTTON3_MASK;
    if (MAP_GPIOPinRead(BUTTON4_GPIO_BASE, BUTTON4_PIN) & BUTTON4_PIN) ui32Data |= BUTTON4_MASK; // button 4

    if (MAP_GPIOPinRead(JOYSTICK_SW_GPIO_BASE, JOYSTICK_SW_PIN) & JOYSTICK_SW_PIN) ui32Data |= JOYSTICK_MASK; // Joystick button


    if(pui8RawState)
    {
        *pui8RawState = (uint8_t)ui32Data; // removed INVERT, buttons are active high
    }

    //
    // Determine the switches that are at a different state than the debounced
    // state.
    //
    ui32Delta = ui32Data ^ g_ui8ButtonStates;

    //
    // Increment the clocks by one.
    //
    ui8SwitchClockA ^= ui8SwitchClockB;
    ui8SwitchClockB = ~ui8SwitchClockB; 

    //
    // Reset the clocks corresponding to switches that have not changed state.
    //
    ui8SwitchClockA &= ui32Delta;
    ui8SwitchClockB &= ui32Delta;

    //
    // Get the new debounced switch state.
    //
    g_ui8ButtonStates &= ui8SwitchClockA | ui8SwitchClockB;
    g_ui8ButtonStates |= (~(ui8SwitchClockA | ui8SwitchClockB)) & ui32Data;

    //
    // Determine the switches that just changed debounced state.
    //
    ui32Delta ^= (ui8SwitchClockA | ui8SwitchClockB);

    //
    // Store the bit mask for the buttons that have changed for return to
    // caller.
    //
    if(pui8Delta)
    {
        *pui8Delta = (uint8_t)ui32Delta;
    }

    //
    // Return the debounced buttons states to the caller.  DONT Invert the bit
    // sense so that a '1' indicates the button is pressed, which is a
    // sensible way to interpret the return value.
    //
    return(g_ui8ButtonStates);
}

//*****************************************************************************
//
//! Initializes the GPIO pins used by the board pushbuttons.
//!
//! This function must be called during application initialization to
//! configure the GPIO pins to which the pushbuttons are attached.  It enables
//! the port used by the buttons and configures each button GPIO as an input
//! with a weak pull-up.
//!
//! \return None.
//
//*****************************************************************************
void
ButtonsInit(void)
{
    //
    // Enable the GPIO port to which the pushbuttons are connected.
    //
    MAP_SysCtlPeripheralEnable(BUTTON1_GPIO_PERIPH);
    MAP_SysCtlPeripheralEnable(BUTTON2_GPIO_PERIPH);
    MAP_SysCtlPeripheralEnable(BUTTON3_GPIO_PERIPH);
    MAP_SysCtlPeripheralEnable(BUTTON4_GPIO_PERIPH);
    MAP_SysCtlPeripheralEnable(JOYSTICK_SW_PERIPH);


    // Set all the directions of each button to inputs
    MAP_GPIODirModeSet(BUTTON1_GPIO_BASE, BUTTON1_PIN, GPIO_DIR_MODE_IN);
    MAP_GPIODirModeSet(BUTTON2_GPIO_BASE, BUTTON2_PIN, GPIO_DIR_MODE_IN);
    MAP_GPIODirModeSet(BUTTON3_GPIO_BASE, BUTTON3_PIN, GPIO_DIR_MODE_IN);
    MAP_GPIODirModeSet(BUTTON4_GPIO_BASE, BUTTON4_PIN, GPIO_DIR_MODE_IN);
    MAP_GPIODirModeSet(JOYSTICK_SW_GPIO_BASE, JOYSTICK_SW_PIN, GPIO_DIR_MODE_IN);

    // Enable pull up resistor for each input
    MAP_GPIOPadConfigSet(BUTTON1_GPIO_BASE, BUTTON1_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    MAP_GPIOPadConfigSet(BUTTON2_GPIO_BASE, BUTTON2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    MAP_GPIOPadConfigSet(BUTTON3_GPIO_BASE, BUTTON3_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    MAP_GPIOPadConfigSet(BUTTON4_GPIO_BASE, BUTTON4_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    MAP_GPIOPadConfigSet(JOYSTICK_SW_GPIO_BASE, JOYSTICK_SW_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Poll current state of each button
    uint8_t initial = 0;
    if (MAP_GPIOPinRead(BUTTON1_GPIO_BASE, BUTTON1_PIN) & BUTTON1_PIN) initial |= BUTTON1_MASK;
    if (MAP_GPIOPinRead(BUTTON2_GPIO_BASE, BUTTON2_PIN) & BUTTON2_PIN) initial |= BUTTON2_MASK;
    if (MAP_GPIOPinRead(BUTTON3_GPIO_BASE, BUTTON3_PIN) & BUTTON3_PIN) initial |= BUTTON3_MASK;
    if (MAP_GPIOPinRead(BUTTON4_GPIO_BASE, BUTTON4_PIN) & BUTTON4_PIN) initial |= BUTTON4_MASK;
    if (MAP_GPIOPinRead(JOYSTICK_SW_GPIO_BASE, JOYSTICK_SW_PIN) & JOYSTICK_SW_PIN) initial |= JOYSTICK_MASK;
    g_ui8ButtonStates = initial;

}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
