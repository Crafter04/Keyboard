#include "HID.h"

typedef struct {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

KeyReport report;

void leere()
{
  for (uint8_t i = 0; i < sizeof(report.keys); i++)
  {
    report.keys[i] = 0;
  }
}

static const uint8_t hidReportDescriptor[] PROGMEM =
{
  0x05, 0x01,
  0x09, 0x06,
  0xA1, 0x01,
  0x85, 0x01,
  0x05, 0x07,
  0x19, 0xE0,
  0x29, 0xE7,
  0x15, 0x00,
  0x25, 0x01,
  0x75, 0x01,
  0x95, 0x08,
  0x81, 0x02,
  0x95, 0x01,
  0x75, 0x08,
  0x81, 0x01,
  0x95, 0x06,
  0x75, 0x08,
  0x15, 0x00,
  0x25, 0x65,
  0x05, 0x07,
  0x19, 0x00,
  0x29, 0x65,
  0x81, 0x00,
  0xC0
};

HIDSubDescriptor node(hidReportDescriptor, sizeof(hidReportDescriptor));

class KeyboardHID_ {
public:
  KeyboardHID_() {
    HID().AppendDescriptor(&node);
  }
} KeyboardHID;

volatile uint32_t lastTrigger = 0;
volatile uint32_t lastTrigger2 = 0;

volatile uint8_t keyBuffer[16];
volatile uint8_t writePos = 0;
volatile uint8_t readPos  = 0;

void sendReport() { HID().SendReport(1, &report, sizeof(report)); }

void processSnapshot(uint8_t snapshot)
{
  leere();
  report.keys[0] = snapshot;
  sendReport();
  delay(5);
  leere();
  sendReport();
}

inline uint8_t readKeys()
{
  uint8_t d = PIND;
  uint8_t c = PINC;
  uint8_t e = PINE;
  uint8_t b = PINB;

  uint8_t result =
        ((b & 0x70) << 1)
      | (d & 0x01)
      | ((d & 0x10) >> 3)
      | ((c & 0x40) >> 4)
      | ((d & 0x80) >> 4)
      | ((e & 0x40) >> 2);

  return ~result; 
}

inline void captureKeys() {
    uint8_t next = (writePos + 1) & 15;
    if (next != readPos) {
        keyBuffer[writePos] = readKeys();
        writePos = next;
    }
}

void pressed() {
    uint32_t now = micros();

    if (now - lastTrigger >= 10000) {
        lastTrigger = now;
        captureKeys();
    }
}

void pressed2() {
    uint32_t now = micros();

    if (now - lastTrigger2 >= 10000) {
        lastTrigger2 = now;
        captureKeys();
    }
}

void setup() {
  leere();

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), pressed, FALLING);
  pinMode(1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(1), pressed2, FALLING);

  pinMode(3,  INPUT_PULLUP);
  pinMode(4,  INPUT_PULLUP);
  pinMode(5,  INPUT_PULLUP);
  pinMode(6,  INPUT_PULLUP);
  pinMode(7,  INPUT_PULLUP);
  pinMode(8,  INPUT_PULLUP);
  pinMode(9,  INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
}

void loop() {
  while (readPos != writePos)
  {
    uint8_t snapshot = keyBuffer[readPos];
    readPos = (readPos + 1) & 15;
    processSnapshot(snapshot);
  }
}