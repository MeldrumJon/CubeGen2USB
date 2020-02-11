#include <SegaController.h>
#include <Nintendo.h>
#include <XInput.h>

// Micro Pins: 3, 0, 2, 4, 5, 1, 6 
// DB9 Pins:   7, 1, 2, 3, 4, 6, 9
SegaController sgController(3, 0, 2, 4, 5, 1, 6);
CGamecubeController gcController(A5); // 5V GC Data

#define SW1 0x01
#define SW2 0x02
#define SW3 0x04
#define SW4 0x08
#define SW5 0x10

static int readSwitches() {
    int val = 0;
    if (digitalRead(A0)) { val |= SW1; }
    if (digitalRead(A1)) { val |= SW2; }
    if (digitalRead(A2)) { val |= SW3; }
    if (digitalRead(A3)) { val |= SW4; }
    if (digitalRead(A4)) { val |= SW5; }
    return ~(val);
}

static int switches = 0;

static void sendGen(word state) {
  static word lastWrite = 0;
  if (state != lastWrite) {
    XInput.setDpad(
      state & SC_BTN_UP,
      state & SC_BTN_DOWN,
      state & SC_BTN_LEFT,
      state & SC_BTN_RIGHT
    );
    XInput.setButton(BUTTON_START, state & SC_BTN_START);
    if (switches & SW1) { // swap a&b, x&y
      XInput.setButton(BUTTON_B, state & SC_BTN_A);
      XInput.setButton(BUTTON_A, state & SC_BTN_B);
      XInput.setButton(BUTTON_Y, state & SC_BTN_X);
      XInput.setButton(BUTTON_X, state & SC_BTN_Y);
    }
    else {
      XInput.setButton(BUTTON_A, state & SC_BTN_A);
      XInput.setButton(BUTTON_B, state & SC_BTN_B);
      XInput.setButton(BUTTON_X, state & SC_BTN_X);
      XInput.setButton(BUTTON_Y, state & SC_BTN_Y);
    }
    XInput.setButton(BUTTON_LB, state & SC_BTN_C);
    if (switches & SW2) { // z imitates right bumper
      XInput.setButton(BUTTON_RB, state & SC_BTN_Z);
    }
    else {
      XInput.setButton(BUTTON_RB, state & SC_BTN_MODE);
    }

    lastWrite = state;
    XInput.send();
  }
}

void sendGC(Gamecube_Report_t &report, bool rumble=false) {
  static Gamecube_Report_t lastReport;
  static bool lastRumble;
  static unsigned long nextAxisUpdate = 0;
  bool doWrite = false;

  unsigned long t = millis();
  if (t > nextAxisUpdate) { // Send axis data every few ms
    XInput.setJoystickX(JOY_LEFT, (int32_t) (report.xAxis - 0x80));
    XInput.setJoystickY(JOY_LEFT, (int32_t) (report.yAxis - 0x80));
    XInput.setJoystickX(JOY_RIGHT, (int32_t) (report.cxAxis - 0x80));
    XInput.setJoystickY(JOY_RIGHT, (int32_t) ~(report.cyAxis - 0x80));
    XInput.setTrigger(TRIGGER_LEFT, (int32_t) (report.left - 0x80));
    XInput.setTrigger(TRIGGER_RIGHT, (int32_t) (report.right - 0x80));
    nextAxisUpdate = t + 3; // every 3ms
    doWrite = true;
  }

  if (report.buttons0 != lastReport.buttons0 
      || report.buttons1 != lastReport.buttons1) {
    XInput.setDpad(report.dup, report.ddown, report.dleft, report.dright);
    XInput.setButton(BUTTON_START, report.start);
    if (switches & SW1) { // swap a&b, x&y
      XInput.setButton(BUTTON_B, report.a);
      XInput.setButton(BUTTON_A, report.b);
      XInput.setButton(BUTTON_Y, report.x);
      XInput.setButton(BUTTON_X, report.y);
    }
    else {
      XInput.setButton(BUTTON_A, report.a);
      XInput.setButton(BUTTON_B, report.b);
      XInput.setButton(BUTTON_X, report.x);
      XInput.setButton(BUTTON_Y, report.y);
    }
    if (switches & SW2) { // use z for bumper instead of L&R buttons
      XInput.setButton(BUTTON_RB, report.z);
    }
    else {
      XInput.setButton(BUTTON_LB, report.l);
      XInput.setButton(BUTTON_RB, report.r);
    }
    doWrite = true;
  }

  if (doWrite) { XInput.send(); }

  if (lastRumble != rumble) {
    gcController.setRumble(rumble);
  }
  
  lastReport = report;
  lastRumble = rumble;
}

void setup() {
  // Controller indicator
  pinMode(LED_BUILTIN, OUTPUT);
  // Switches
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  // Gamecube joystick range
  XInput.setTriggerRange(-128, 127);
  XInput.setRange(JOY_LEFT, -128, 127);
  XInput.setRange(JOY_RIGHT, -128, 127);
  // Wait for all controls before sending
  XInput.setAutoSend(false);

  XInput.begin();
}

void loop() {
  switches = readSwitches();

  if (gcController.read()) {
    digitalWrite(LED_BUILTIN, HIGH); // Controller plugged in
    Gamecube_Report_t report = gcController.getReport();
    sendGC(report, (XInput.getRumble() > 15)); // only rumble with when XBox's large motor should rumble.
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
  XInput.releaseAll();
  XInput.send();
  delay(500); // Try again in half a second
}
