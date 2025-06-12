//*****************************************************************************
//
// buttons.h - Prototypes for the evaluation board buttons driver.
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

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Defines for the hardware resources used by the pushbuttons.
//
// The switches are on the following ports/pins:
//
// PF4 - Left Button
// PF0 - Right Button
//
// The switches tie the GPIO to ground, so the GPIOs need to be configured
// with pull-ups, and a value of 0 means the switch is pressed.
//
//*****************************************************************************
// Button GPIO Periph Enables
#define BUTTON1_GPIO_PERIPH     SYSCTL_PERIPH_GPIOF
#define BUTTON2_GPIO_PERIPH     SYSCTL_PERIPH_GPIOF
#define BUTTON3_GPIO_PERIPH     SYSCTL_PERIPH_GPIOB
#define BUTTON4_GPIO_PERIPH     SYSCTL_PERIPH_GPIOC
#define JOYSTICK_SW_PERIPH      SYSCTL_PERIPH_GPIOA

// Button GPIO Bases
#define BUTTON1_GPIO_BASE       GPIO_PORTF_BASE
#define BUTTON2_GPIO_BASE       GPIO_PORTF_BASE
#define BUTTON3_GPIO_BASE       GPIO_PORTB_BASE
#define BUTTON4_GPIO_BASE       GPIO_PORTC_BASE
#define JOYSTICK_SW_GPIO_BASE   GPIO_PORTA_BASE

// Button Pin Numbers
#define BUTTON1_PIN             GPIO_PIN_2
#define BUTTON2_PIN             GPIO_PIN_3
#define BUTTON3_PIN             GPIO_PIN_3
#define BUTTON4_PIN             GPIO_PIN_4
#define JOYSTICK_SW_PIN         GPIO_PIN_5

//
#define ALL_BUTTONS   (BUTTON1_PIN | BUTTON2_PIN | BUTTON3_PIN | BUTTON4_PIN | JOYSTICK_SW_PIN)
#define PORTF_BUTTONS (BUTTON1_PIN | BUTTON2_PIN)

// Masks
#define BUTTON1_MASK   0x01
#define BUTTON2_MASK   0x02
#define BUTTON3_MASK   0x04
#define BUTTON4_MASK   0x08
#define JOYSTICK_MASK  0x10

//*****************************************************************************
//
// Useful macros for detecting button events.
//
//*****************************************************************************
#define BUTTON_PRESSED(button, buttons, changed)                              \
        (((button) & (changed)) && ((button) & (buttons)))

#define BUTTON_RELEASED(button, buttons, changed)                             \
        (((button) & (changed)) && !((button) & (buttons)))

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Functions exported from buttons.c
//
//*****************************************************************************
extern void ButtonsInit(void);
extern uint8_t ButtonsPoll(uint8_t *pui8Delta,
                             uint8_t *pui8Raw);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Prototypes for the globals exported by this driver.
//
//*****************************************************************************

#endif // __BUTTONS_H__
