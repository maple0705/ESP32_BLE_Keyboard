#include <M5StickC.h>
//#include <BleKeyboard.h>
#include <BleKeyboard_Raw.h>

BleKeyboard bleKeyboard("M5StickC KB");

// for debug
void drawScreen(String Text1 = "", String Text2 = "", String Text3 = "") {

  M5.Lcd.setRotation(3);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("M5StickC KB");

  M5.Lcd.setCursor(0, 16);
  M5.Lcd.println("BAT: " + String(M5.Axp.GetBatVoltage()) + " V");

  M5.Lcd.setCursor(0, 32);
  M5.Lcd.println(Text1);

  M5.Lcd.setCursor(0, 48);
  M5.Lcd.println(Text2);

  M5.Lcd.setCursor(0, 64);
  M5.Lcd.println(Text3);
}

// Event follows the format below:
// AABCCDD;
// AA: Keyboard address (2byte)
// B: Event type (1byte)
// CC: arg1 of Event (2byte)
// DD: arg2 of Event (2byte, may be not outputed)
// ; is termination charactor. not included in "Event" arg
void sendKeyEvent(String Event) {
  char code[3] = {0, 0, 0};
  byte keyCode = 0; // to be sent to host
  byte asciiCode = 0;
  int bitidx = 0; // to find modifier key

  // convert string of HEX to byte
  Event.substring(3, 5).toCharArray(code, 3);
  keyCode = (byte)strtol(code, 0, 16);
  if( Event.length() >= 7 ) {
    Event.substring(5, 7).toCharArray(code, 3);
    asciiCode = (byte)strtol(code, 0, 16);
  }

  switch( Event.charAt(2) ) {
    case 'K': // key down event(6 or 8byte)
      bleKeyboard.pressRaw(keyCode);
      break;

    case 'k': // key up event(6byte)
      bleKeyboard.releaseRaw(keyCode);
      break;

    case 'M': // modifier key down event(6byte)
      while (keyCode >>= 1) bitidx++;  // if keyCode == 1 => Left Shift(idx == 0)
      bleKeyboard.pressRaw(bitidx + 0xE0); //convert to actual Usage ID
      break;

    case 'm': // modifier key up event(6byte)
      while (keyCode >>= 1) bitidx++;  // if keyCode == 1 => Left Shift(idx == 0)
      bleKeyboard.releaseRaw(bitidx + 0xE0); //convert to actual Usage ID
      break;

    default:  // 'A', 'a', 'L', 'R' are ignored
      break;
  }

  // debug: print key code and ascii code directly
  if( Event.length() >= 7 ) {
    drawScreen(Event, String(keyCode), String(asciiCode));
  } else {
    drawScreen(Event, String(keyCode));
  }
}

void setup() {

  Serial1.begin(9600, SERIAL_8N1, 36, 0);
  bleKeyboard.begin();
  M5.begin();

  drawScreen();
}

void loop() {

  if( Serial1.available() > 0 ) {
    String inEvent = Serial1.readStringUntil(';');

    if( inEvent != NULL ) {
      if( bleKeyboard.isConnected() ) {
        sendKeyEvent(inEvent);
      }
    }
  }

}
