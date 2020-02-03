#include <SegaController.h>
#include <Nintendo.h>
#include <HID-Project.h>

// Micro Pins: 3, 0, 2, 4, 5, 1, 6 
// DB9 Pins:   7, 1, 2, 3, 4, 6, 9
SegaController sgController(3, 0, 2, 4, 5, 1, 6);
CGamecubeController gcController(A5); // 5V GC Data

// dPad -> HID dPad
static int8_t dPadDir(bool up, bool down, bool left, bool right) {
  if (up && right) { return GAMEPAD_DPAD_UP_RIGHT; }
  else if (up && left) { return GAMEPAD_DPAD_UP_LEFT; }
  else if (down && right)  { return GAMEPAD_DPAD_DOWN_RIGHT; }
  else if (down && left) { return GAMEPAD_DPAD_DOWN_LEFT; }
  else if (up) { return GAMEPAD_DPAD_UP; }
  else if (right) { return GAMEPAD_DPAD_RIGHT; }
  else if (down) { return GAMEPAD_DPAD_DOWN; }
  else if (left)  { return GAMEPAD_DPAD_LEFT; }
  else { return GAMEPAD_DPAD_CENTERED; }
}

static void sendGen(word state) {
  static word lastWrite = 0;
  if (state != lastWrite) {
    Gamepad.dPad1(
      dPadDir(
        state & SC_BTN_UP,
        state & SC_BTN_DOWN,
        state & SC_BTN_LEFT,
        state & SC_BTN_RIGHT
      )
    );
    Gamepad.buttons(0x00UL | state >> 5);

    lastWrite = state;
    Gamepad.write();
  }
}

void sendGC(Gamecube_Report_t &report)
{
  // The two control sticks
  Gamepad.xAxis((report.xAxis - 0x80) << 8);
  Gamepad.yAxis(~(report.yAxis - 0x80) << 8); // y stick needs to be inverted
  Gamepad.rxAxis((report.cxAxis - 0x80) << 8);
  Gamepad.ryAxis((report.cyAxis - 0x80) << 8);
  // you can merge L/R (PS3 controller uses this methode too)
  // or you can also seperate the triggers
  // for Windows calibration comment out the l/r buttons below
  // (because l/r triggers the wizard)
  //Gamepad.zAxis(abs(gc_report.left - gc_report.right) - 0x80);
  Gamepad.zAxis(report.left - 0x80);
  Gamepad.rzAxis(report.right - 0x80);

  Gamepad.dPad1(
    dPadDir(
      report.dup,
      report.ddown,
      report.dleft,
      report.dright
    )
  );
  // 8 Gamecube buttons
  Gamepad.buttons(0x00UL | (report.buttons0 & 0x1F) | ((report.buttons1 & 0x70) << 1));

  Gamepad.write();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Gamepad.begin();
}

void loop() {
  if (gcController.read()) {
    digitalWrite(LED_BUILTIN, HIGH); // Controller plugged in
    Gamecube_Report_t report = gcController.getReport();
    sendGC(report);
    delay(20);
    return;
  }
  word state = sgController.getState();
  if (state & SC_CTL_ON) { // Controller plugged in
    digitalWrite(LED_BUILTIN, HIGH);
    sendGen(state);
    delay(10);
    return;
  }

  // No controller
  digitalWrite(LED_BUILTIN, LOW);
  Gamepad.releaseAll();
  Gamepad.write();
  delay(500); // Try again in half a second
}
