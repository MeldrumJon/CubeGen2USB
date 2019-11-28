/*
  Copyright (c) 2014-2015 NicoHood
  See the readme for credit to other people.

  Gamepad example
  Press a button and demonstrate Gamepad actions

  You can also use Gamepad1,2,3 and 4 as single report.
  This will use 1 endpoint for each gamepad.

  See HID Project documentation for more infos
  https://github.com/NicoHood/HID/wiki/Gamepad-API
*/

#include <SegaController.h>
#include "HID-Project.h"

// Controller DB9 pins (looking face-on to the end of the plug):
//
// 5 4 3 2 1
//  9 8 7 6
//
// Connect pin 5 to +5V and pin 8 to GND
// Connect the remaining pins to digital I/O pins (see below)

// DB9-1 -> 2 -> yellow
// DB9-2 -> 3 -> gray
// DB9-3 -> 4 -> blue
// DB9-4 -> 5 -> orange
// DB9-5 -> VCC -> red
// DB9-6 -> 6 -> brown
// DB9-7 -> 8 -> green
// DB9-8 -> GND -> black
// DB9-9 -> 7 -> white

// RED -> VCC

// Specify the Arduino pins that are Vconnected to
// DB9 Pin 7, DB9 Pin 1, DB9 Pin 2, DB9 Pin 3, DB9 Pin 4, DB9 Pin 6, DB9 Pin 9
static SegaController sgController(8, 2, 3, 4, 5, 6, 7);

// dPad -> HID dPad
static int8_t _getDPadDir(uint16_t up, uint16_t down, uint16_t left, uint16_t right) {
    if (up && right) { return GAMEPAD_DPAD_UP_RIGHT; }
    else if (down && right)  { return GAMEPAD_DPAD_DOWN_RIGHT; }
    else if (down && left) { return GAMEPAD_DPAD_DOWN_LEFT; }
    else if (up && left) { return GAMEPAD_DPAD_UP_LEFT; }
    else if (up) { return GAMEPAD_DPAD_UP; }
    else if (right) { return GAMEPAD_DPAD_RIGHT; }
    else if (down) { return GAMEPAD_DPAD_DOWN; }
    else if (left)  { return GAMEPAD_DPAD_LEFT; }
    else { return GAMEPAD_DPAD_CENTERED; }
}

static void _sendBtnChange(uint8_t cur, uint8_t last, uint8_t btn) {
  if (cur && !last) { Gamepad.press(btn); }
  else if (!cur && last) { Gamepad.release(btn); }
}

static void _sendGenesis(int_fast16_t state) {
  static int_fast16_t lastState = 0;
  if (state != lastState) {
    int_fast8_t dPad = _getDPadDir(state & SC_BTN_UP, state & SC_BTN_DOWN, state & SC_BTN_LEFT, state & SC_BTN_RIGHT);
    Gamepad.dPad1(dPad);
    _sendBtnChange(state&SC_BTN_START, lastState&SC_BTN_START, 1);
    _sendBtnChange(state&SC_BTN_A, lastState&SC_BTN_A, 2);
    _sendBtnChange(state&SC_BTN_B, lastState&SC_BTN_B, 3);
    _sendBtnChange(state&SC_BTN_C, lastState&SC_BTN_C, 4);
    _sendBtnChange(state&SC_BTN_X, lastState&SC_BTN_X, 5);
    _sendBtnChange(state&SC_BTN_Y, lastState&SC_BTN_Y, 6);
    _sendBtnChange(state&SC_BTN_Z, lastState&SC_BTN_Z, 7);
    _sendBtnChange(state&SC_BTN_MODE, lastState&SC_BTN_MODE, 8);
    Gamepad.write();
  }
  lastState = state;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Gamepad.begin();
}

void loop() {
  int_fast16_t state = sgController.getState();
  if (state & SC_CTL_ON) { // Controller plugged in
    digitalWrite(LED_BUILTIN, HIGH);
    _sendGenesis(state);
  }
  else { // No controller
    digitalWrite(LED_BUILTIN, LOW);
    delay(500); // Try again in half a second
  }
}
