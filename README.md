# CubeGen2USB

GameCube and Sega Genesis to USB adapter.  Works for both XInput and DInput 
(depending on which Arduino sketch is used). Only one controller may be used at 
a time (if both controllers are plugged in, the GameCube controller is used).

![Photo of the adapter](../assets/Photo.jpg)

## Parts

- Arduino Micro
- 1x [DB9 Male Connector](https://www.mouser.com/ProductDetail/571-2301843-1)
- 1x GameCube controller extension cable, cut in half with wires stripped.
- 5x [SPDT Slide Switches](https://www.amazon.com/gp/product/B01M1CU2B0/)
- 1x [Bi-Direction 3.3V-5V Logic Level Converter Module](https://www.amazon.com/gp/product/B07F7W91LC/)
	- or 1x BSS138 MOSFET and 2x 10k resistors.
- Breakaway Headers

Be careful when soldering the GameCube extension cable wires onto the PCB.  Use 
[this](https://github.com/NicoHood/Nintendo/wiki/Gamecube#hardware) as a reference, 
but disregard the wire colors.  Use a continuity tester to determine which wire 
is connected to each pin.

## PCB

KiCAD files are in the [hardware](./hardware) folder.

![Circuit Schematic](../assets/Schematic.png)

3rd-party KiCAD libaries used:

- [Arduino KiCAD Library](https://github.com/Alarm-Siren/arduino-kicad-library)

## Programming

### DInput Setup

#### Required Libraries

- [SegaController by Jon Thysell](https://github.com/jonthysell/SegaController)
- Nintendo by NicoHood
- HID-Project by NicoHood

### XInput Setup

#### Hardware Setup

1. Download
[XInput USB Core for Arduino AVR](https://github.com/dmadison/ArduinoXInput_AVR) 
and extract it to `Arduino/hardware/xinput/avr`.
1. Change the board in Arduino IDE to "Arduino Micro w/ XInput".
1. Burn the bootloader using an In-circuit Serial Programmer ([Another Arduino 
can be used as an ISP](https://www.arduino.cc/en/tutorial/arduinoISP)).

Once the bootloader is burned, you will have to press the reset button when 
uploading sketches or simply upload using the ISP.

#### Required Libraries

- [SegaController by Jon Thysell](https://github.com/jonthysell/SegaController)
- Nintendo by NicoHood
- XInput by David Madison

## Features

The five switches can be used to control how buttons are mapped to the emulated 
XBox 360 controller. By default:

| XBox     | GameCube         | Genesis |
| -------- | ---------------- | ------- |
| Start    | Start            | Start   |
| A        | A                | A       |
| B        | B                | B       |
| X        | X                | X       |
| Y        | Y                | Y       |
| L Bumper | L Trigger Button | C       |
| R Bumper | R Trigger Button | Mode    |

- SW1: Swap A&B, X&Y
- SW2: Z maps to R Bumper
- SW3: 
- SW4: 
- SW5: 

