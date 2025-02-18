#include <M5StickC.h>
//#include <BleKeyboard.h>
#include <BleKeyboard_Raw.h>

#define QUEUE_SIZE  10

BleKeyboard bleKeyboard("M5StickC KB");

unsigned long preEventTime;
String eventQueue[QUEUE_SIZE];
unsigned long eventTimeDeltaQueue[QUEUE_SIZE];
int queueIdx = -1;  // arrayIdx >= 0 => some events are in array

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
// ; is termination charactor. not included in Event arg
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

void sendKeyEventQueue() {
  int i;

  if( queueIdx < 0 ) return;

  for( i = 0; i <= queueIdx; i++ ) {
    if( eventQueue[i] == "" ) continue;
    sendKeyEvent(eventQueue[i]);
  }
  deleteQueue();
  return;
}

// return 0 if queue is full
// return 1 if Event is added to queue
int addToQueue(String Event, unsigned long timeDelta) {

  if( queueIdx == QUEUE_SIZE - 1 ) return 0;
  queueIdx++;
  eventQueue[queueIdx] = Event;
  eventTimeDeltaQueue[queueIdx] = timeDelta;
  return 1;
}

// return 0 if anything was deleted
// return 1 if some events are deleted
// queueIdx is not changed if even 1 is returned
int deleteFromQueue(String Event, int deleteOnce) {
  int i;
  int deleted = 0;

  for( i = 0; i < QUEUE_SIZE; i++ ) {
    if( eventQueue[i] == Event ) {
        eventQueue[i] = "";
        eventTimeDeltaQueue[i] = 0;
        deleted = 1;
        if( deleteOnce ) return deleted;
    }
  }
  return deleted;
}

void deleteQueue() {
  int i;

  for( i = 0; i < QUEUE_SIZE; i++ ) {
    eventQueue[i] = "";
    eventTimeDeltaQueue[i] = 0;
  }
  queueIdx = -1;
  return;
}

void preventChattering() {
  int i, j;

  if( queueIdx < 1 ) return;

  for( i = 1; i <= queueIdx; i++ ) {
    for( j = i + 1; j <= queueIdx; j++ ) {
      if( eventQueue[i].substring(3, 5) == eventQueue[j].substring(3, 5) ) {
        eventQueue[i] = eventQueue[j] = "";
        eventTimeDeltaQueue[i] = eventTimeDeltaQueue[j] = 0;
      }
    }
  }
  return;
}

void setup() {
  Serial.begin(115200); // debug
  Serial1.begin(115200, SERIAL_8N1, 36, 0);
  bleKeyboard.begin();
  M5.begin();

  drawScreen();
}

void loop() {

  if( Serial1.available() > 0 ) {
    unsigned long now;
    String inEvent = Serial1.readStringUntil(';');
    if( inEvent != NULL ) {
      now = millis();
      addToQueue(inEvent, now - preEventTime);
      preEventTime = now;

      while( millis() - preEventTime < 3 ) {
        if( Serial1.available() > 0 ) {
          inEvent = Serial1.readStringUntil(';');
          if( inEvent != NULL ) {
            now = millis();
            addToQueue(inEvent, now - preEventTime);
            preEventTime = now;
          }
        }
      }

      Serial.println("Start1");
      for( int i = 0; i <= queueIdx; i++ ) {
        Serial.println(eventQueue[i] + " " + String(eventTimeDeltaQueue[i]) + "ms");
      }
      Serial.println("End1");

      preventChattering();

      Serial.println("Start2");
      for( int i = 0; i <= queueIdx; i++ ) {
        Serial.println(eventQueue[i] + " " + String(eventTimeDeltaQueue[i]) + "ms");
      }
      Serial.println("End2");
      Serial.println("");

      if( bleKeyboard.isConnected() ) {
        sendKeyEventQueue();
      }
    }
  }

}
