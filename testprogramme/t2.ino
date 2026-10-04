#include "HID.h"



typedef struct {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

KeyReport report; 

static const uint8_t hidReportDescriptor[] PROGMEM = 
{ 
  0x05, 0x01, // Usage Page (Generic Desktop) 
  0x09, 0x06, // Usage (Keyboard) 
  0xA1, 0x01, // Collection (Application) 
  0x85, 0x01, // REPORT ID (1) 
  0x05, 0x07, // Usage Page (Keyboard) 
  0x19, 0xE0, // Usage Minimum (224) 
  0x29, 0xE7, // Usage Maximum (231) 
  0x15, 0x00, // Logical Minimum (0) 
  0x25, 0x01, // Logical Maximum (1) 
  0x75, 0x01, // Report Size (1) 
  0x95, 0x08, // Report Count (8) 
  0x81, 0x02, // Input (Data,Var,Abs) -> Modifier Byte 
  0x95, 0x01, // Report Count (1) 
  0x75, 0x08, // Report Size (8) 
  0x81, 0x01, // Input (Const) -> Reserved 
  0x95, 0x06, // Report Count (6) 
  0x75, 0x08, // Report Size (8) 
  0x15, 0x00, // Logical Minimum (0) 
  0x25, 0x65, // Logical Maximum (101) 
  0x05, 0x07, // Usage Page (Keyboard) 
  0x19, 0x00, // Usage Minimum (0) 
  0x29, 0x65, // Usage Maximum (101) 
  0x81, 0x00, // Input (Data,Array) 
  0xC0 // End Collection 
}; 

HIDSubDescriptor node(hidReportDescriptor, sizeof(hidReportDescriptor)); 

class KeyboardHID_ {
public:
  KeyboardHID_() {
    HID().AppendDescriptor(&node);
  }
} KeyboardHID;

const int buttonPin = 2;
volatile bool buttonChanged = false;

void pressed() {
  buttonChanged = true;
}

void setup() { 
  pinMode(buttonPin, INPUT_PULLUP);
  memset(&report, 0, sizeof(report));
  attachInterrupt(digitalPinToInterrupt(2), pressed, CHANGE);
} 

void sendReport() {
  HID().SendReport(1, &report, sizeof(report)); 
}
bool lastState = HIGH;
volatile unsigned long lastInterruptTime = 0;

void loop() { 
  if (buttonChanged) {
    buttonChanged = false;
    unsigned long now = millis();
    if (now - lastInterruptTime > 5) {
      bool currentState = digitalRead(buttonPin);
      if (currentState == lastState) return;
      lastState = currentState;

      if (currentState == LOW) {
        report.keys[0] = 0x04;
        sendReport();
      } else {
        report.keys[0] = 0x00;
        sendReport();
      }
    }
    lastInterruptTime = now;
  }
}