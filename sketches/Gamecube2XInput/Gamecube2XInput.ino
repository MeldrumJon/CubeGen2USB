#include <XInput.h>
#include "Nintendo.h"

// Current wire colors:
// Yellow  4  GND
// Black   3  GND
// White   1  5V
// Green   6  3.3V
// Red     2  DATA

CGamecubeController gcController(A0);

void _sendGamecube(Gamecube_Report_t &report, bool rumble=false) {
  static Gamecube_Report_t last_report;
  static bool last_rumble;
  static unsigned long next_axis_time = 0;
  uint8_t do_write = 0;

  unsigned long t = millis();
  if (t > next_axis_time) { // Send axis data every few ms
    XInput.setJoystickX(JOY_LEFT, (int32_t) (report.xAxis - 0x80));
    XInput.setJoystickY(JOY_LEFT, (int32_t) (report.yAxis - 0x80));
    XInput.setJoystickX(JOY_RIGHT, (int32_t) (report.cxAxis - 0x80));
    XInput.setJoystickY(JOY_RIGHT, (int32_t) ~(report.cyAxis - 0x80));
    XInput.setTrigger(TRIGGER_LEFT, (int32_t) (report.left - 0x80));
    XInput.setTrigger(TRIGGER_RIGHT, (int32_t) (report.right - 0x80));
    next_axis_time = t + 3; // every 3ms
    do_write = 1;
  }

  if (report.buttons0 != last_report.buttons0 
      || report.buttons1 != last_report.buttons1) {
    XInput.setDpad(report.dup, report.ddown, report.dleft, report.dright);
    XInput.setButton(BUTTON_START, report.start);
    XInput.setButton(BUTTON_A, report.a);
    XInput.setButton(BUTTON_B, report.b);
    XInput.setButton(BUTTON_X, report.x);
    XInput.setButton(BUTTON_Y, report.y);
    XInput.setButton(BUTTON_LB, report.l);
    XInput.setButton(BUTTON_RB, report.r);
    do_write = 1;
  }

  if (do_write) { XInput.send(); }

  if (last_rumble != rumble) {
    gcController.setRumble(rumble);
  }
  
  last_report = report;
  last_rumble = rumble;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  XInput.setTriggerRange(-128, 127);
  XInput.setRange(JOY_LEFT, -128, 127);
  XInput.setRange(JOY_RIGHT, -128, 127);
  XInput.setAutoSend(false);  // Wait for all controls before sending
  XInput.begin();
}

void loop() {
  // Try to read the controller data
  if (gcController.read()) {
    digitalWrite(LED_BUILTIN, LOW);
    // Send controller data to the USB interface
    Gamecube_Report_t report = gcController.getReport();
    _sendGamecube(report, (XInput.getRumble() > 15));
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500); // Try again in half a second
  }
}
