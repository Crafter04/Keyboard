#include "HID.h"

typedef struct {
  uint8_t modifiers;
  uint8_t keys[1];
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
};

KeyboardHID_ KeyboardHID;

// (HID Reserved 0xE8-0xEF - sonst kann man bei 0x00 in eine modifiere auswahl gehen - das würde aber flow Stören)

#define CMD_TOGGLE_LCTRL  0xE8
#define CMD_TOGGLE_LSHIFT 0xE9
#define CMD_TOGGLE_LALT   0xEA
#define CMD_TOGGLE_LGUI   0xEB
#define CMD_TOGGLE_RCTRL  0xEC
#define CMD_TOGGLE_RSHIFT 0xED
#define CMD_TOGGLE_RALT   0xEE
#define CMD_TOGGLE_RGUI   0xEF

#define MOD_LCTRL  0x01
#define MOD_LSHIFT 0x02
#define MOD_LALT   0x04
#define MOD_LGUI   0x08
#define MOD_RCTRL  0x10
#define MOD_RSHIFT 0x20
#define MOD_RALT   0x40
#define MOD_RGUI   0x80

uint8_t heldModifiers = 0;

void sendReport() {
  HID().SendReport(1, &report, sizeof(report));
}

void processSnapshot(uint8_t snapshot)
{
  uint8_t modifier = 0;

  switch (snapshot)
  {
    case CMD_TOGGLE_LCTRL:  modifier = MOD_LCTRL;  break;
    case CMD_TOGGLE_LSHIFT: modifier = MOD_LSHIFT; break;
    case CMD_TOGGLE_LALT:   modifier = MOD_LALT;   break;
    case CMD_TOGGLE_LGUI:   modifier = MOD_LGUI;   break;
    case CMD_TOGGLE_RCTRL:  modifier = MOD_RCTRL;  break;
    case CMD_TOGGLE_RSHIFT: modifier = MOD_RSHIFT; break;
    case CMD_TOGGLE_RALT:   modifier = MOD_RALT;   break;
    case CMD_TOGGLE_RGUI:   modifier = MOD_RGUI;   break;
  }

  if (modifier != 0)
  {
    heldModifiers ^= modifier;
    leere();
    report.modifiers = heldModifiers;
    sendReport();
    return;
  }

  leere();
  report.modifiers = heldModifiers;
  report.keys[0] = snapshot;
  sendReport();

  delay(5);
  // Tasten loslassen
  leere();
  report.modifiers = heldModifiers;
  sendReport();
}


volatile uint8_t keyBuffer[16];
volatile uint8_t writePos = 0;
volatile uint8_t readPos  = 0;

volatile bool trigger1Armed = true;
volatile bool trigger2Armed = true;

uint32_t trigger1HighSince = 0;
uint32_t trigger2HighSince = 0;

const uint32_t RELEASE_TIME = 10000;


// Bit 0 = Pin 3   Bit 4 = Pin 7
// Bit 1 = Pin 4   Bit 5 = Pin 8
// Bit 2 = Pin 5   Bit 6 = Pin 9
// Bit 3 = Pin 6   Bit 7 = Pin 10
//
// INPUT_PULLUP: HIGH = nicht gedrückt, LOW = gedrückt
// -> also Ergebnis invert

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


void captureKeys()
{
  uint8_t next = (writePos + 1) & 15;

  if (next != readPos)
  {
    keyBuffer[writePos] = readKeys();
    writePos = next;
  }
}


void pressed()
{
  if (trigger1Armed)
  {
    trigger1Armed = false;
    captureKeys();
  }
}

void pressed2()
{
  if (trigger2Armed)
  {
    trigger2Armed = false;
    captureKeys();
  }
}

// ___________________________________________________________________________________________________________________________________
void setup()
{
  report.modifiers = 0;
  report.reserved = 0;
  leere();

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), pressed2, FALLING);

  pinMode(1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(1), pressed, FALLING);

  pinMode(3,  INPUT_PULLUP);
  pinMode(4,  INPUT_PULLUP);
  pinMode(5,  INPUT_PULLUP);
  pinMode(6,  INPUT_PULLUP);
  pinMode(7,  INPUT_PULLUP);
  pinMode(8,  INPUT_PULLUP);
  pinMode(9,  INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
}

void loop()
{
  uint32_t now = micros();

  // Trigger re-arm
  if (!trigger1Armed)
  {
    if (digitalRead(1) == HIGH)
    {
      if (trigger1HighSince == 0)
        trigger1HighSince = now;
      else if ((uint32_t)(now - trigger1HighSince) >= RELEASE_TIME)
      {
        noInterrupts();
        if (digitalRead(1) == HIGH)
        {
          trigger1Armed = true;
          trigger1HighSince = 0;
        }
        interrupts();
      }
    }
    else
    {
      trigger1HighSince = 0;
    }
  }

  if (!trigger2Armed)
  {
    if (digitalRead(2) == HIGH)
    {
      if (trigger2HighSince == 0)
        trigger2HighSince = now;
      else if ((uint32_t)(now - trigger2HighSince) >= RELEASE_TIME)
      {
        noInterrupts();
        if (digitalRead(2) == HIGH)
        {
          trigger2Armed = true;
          trigger2HighSince = 0;
        }
        interrupts();
      }
    }
    else
    {
      trigger2HighSince = 0;
    }
  }

  // Puffer arbeiten
  while (readPos != writePos)
  {
    uint8_t snapshot = keyBuffer[readPos];
    readPos = (readPos + 1) & 15;

    processSnapshot(snapshot);
  }
}
