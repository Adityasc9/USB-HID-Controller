# USB HID Game Controller

**Platform:** Tiva TM4C123GH6PM LaunchPad

## Project Overview

This project implements a USB HID (Human Interface Device) game controller using the Tiva TM4C123GH6PM LaunchPad. The controller is recognized by a PC as a standard gamepad, compatible with games like Rocket League. It uses basic components from a starter electronics kit: 5 digital buttons, a joystick for directional movement, and a potentiometer acting as an analog trigger.

## Goals

- Build a fully functional USB HID game controller playable on a PC.
- Learn about USB device development, HID descriptors, ADC and GPIO programming on ARM Cortex-M4.
- Explore embedded system development beyond beginner-friendly platforms like Arduino.

## Features

- Directional Input: Analog joystick (X, Y axes) for movement.
- Button Actions: 5 buttons mapped to standard HID gamepad inputs.
- Analog Trigger: Potentiometer serves as a smooth analog trigger.
- USB HID Interface: Shows up on the PC as a standard game controller.
- Rocket League Compatibility: Tested and playable in games using native HID support.

## Components

| Component                   | Quantity | Purpose                     |
| --------------------------- | -------- | --------------------------- |
| Tiva TM4C123GH6PM LaunchPad | 1        | Main microcontroller board  |
| Joystick Module             | 1        | Movement input (X/Y + SW)   |
| Push Buttons                | 4        | Gamepad action buttons      |
| Potentiometer               | 1        | Analog trigger input        |
| USB Cable                   | 1        | Communication and power     |
| Breadboard and Jumper Wires | -        | Prototyping and connections |

## Pinout

| Function            | Tiva Pin   | Notes          |
| ------------------- | ---------- | -------------- |
| Potentiometer       | PE4 (AIN9) | Analog trigger |
| Joystick VRx        | PE5 (AIN8) | X-axis         |
| Joystick VRy        | PE1 (AIN2) | Y-axis         |
| Joystick SW (Click) | PA5        | Digital button |
| Button 1            | PF2        | Action button  |
| Button 2            | PF3        | Action button  |
| Button 3            | PB3        | Action button  |
| Button 4            | PC4        | Action button  |

## Installation & Setup

1. Clone or download this repository.
2. Open the project in Code Composer Studio (CCS) or another Tiva-compatible IDE.
3. Ensure the TivaWare SDK is installed and referenced properly in your project.
4. Build the project and flash the firmware to the Tiva board via USB.
5. Connect the controller to a PC via USB. It should appear as a standard game controller.
6. Use the "Set up USB Game Controllers" utility on Windows (`joy.cpl`) to test inputs.

## (Optional) Setup for Steam

7. Turn on 'Enable Steam input for generic controllers'
8. Go through the 'Configure Inputs' section and map inputs to your desired fields.

## Design Choices

### Why Build a USB Game Controller?

- Personal motivation: Existing controller broke, and I wanted to create a custom one to play Rocket League.
- Skill development: This project explores advanced concepts such as USB HID, embedded C, ADC, and GPIO.
- Resume project: Demonstrates understanding of USB protocol and low-level MCU programming.

### Why Use the Tiva LaunchPad?

- I already own the Tiva LaunchPad and wanted to move beyond Arduino-based projects to real world embedded projects.
- Tiva has minimal tutorials, making it a more challenging and rewarding learning experience.
- Forced reliance on Tiva datasheets and official documentation, which strengthens my embedded skills.

### Why This Layout?

- I had access to one joystick module, five buttons, and one potentiometer.
- Simple yet expandable layout; future versions could add more inputs, wireless support, or a custom PCB.

## Dependencies

- TivaWare USB Library for HID class implementation.
- No third-party libraries used. Everything is written in embedded C using low-level register access.

## Testing

- Verified analog values through serial debugging before USB integration.
- Confirmed functionality with `joy.cpl` (Game Controller Settings on Windows).
- Confirmed functionality with `https://hardwaretester.com/gamepad` (Online Controller Tester).
- Successfully used the controller to play Rocket League.

## Future Improvements

- Add wireless Bluetooth HID capabilities
- Design a custom PCB to replace breadboard
- Implement a vibration motor for ingame feedback
