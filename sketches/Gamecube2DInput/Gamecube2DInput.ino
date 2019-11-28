#include "HID-Project.h"
#include "Nintendo.h"

// Current wire colors:
// Yellow  4  GND
// Black   3  GND
// White   1  5V
// Green   6  3.3V
// Red     2  DATA

CGamecubeController gcController(A0);

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

void _sendGamecube(Gamecube_Report_t &report)
{
  static Gamecube_Report_t last_report;
  static unsigned long next_axis_time = 0;
  uint8_t do_write = 0;

  unsigned long t = millis();
  if (t > next_axis_time) { // Send axis data every few ms
    Gamepad.xAxis(((int16_t) (report.xAxis - 0x80)) << 8);
    Gamepad.yAxis(((int16_t) ~(report.yAxis - 0x80)) << 8); // y stick needs to be inverted
    Gamepad.rxAxis(((int16_t) (report.cxAxis - 0x80)) << 8);
    Gamepad.ryAxis(((int16_t) (report.cyAxis - 0x80)) << 8);
    Gamepad.zAxis(report.left - 0x80);
    Gamepad.rzAxis(report.right - 0x80);
    next_axis_time = t + 5; // every 5ms
    do_write = 1;
  }

  if (report.buttons0 != last_report.buttons0 
      || report.buttons1 != last_report.buttons1) {
    int8_t dPad = _getDPadDir(report.dup, report.ddown, report.dleft, report.dright);
    Gamepad.dPad1(dPad);
    _sendBtnChange(report.start, last_report.start, 1);
    _sendBtnChange(report.b, last_report.b, 2);
    _sendBtnChange(report.a, last_report.a, 3);
    _sendBtnChange(report.x, last_report.x, 4);
    _sendBtnChange(report.y, last_report.y, 6);
    _sendBtnChange(report.z, last_report.z, 7);
    _sendBtnChange(report.r, last_report.r, 8);
    _sendBtnChange(report.l, last_report.l, 9);
    do_write = 1;
  }

  if (do_write) { Gamepad.write(); }

  last_report = report;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Gamepad.begin(); // Sends a clean report to the host. This is important on any Arduino type.
}

void loop() {
  // Try to read the controller data
  if (gcController.read()) {
    digitalWrite(LED_BUILTIN, HIGH);
    // Send controller data to the USB interface
    Gamecube_Report_t report = gcController.getReport();
    _sendGamecube(report);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500); // Try again in half a second
  }
}
