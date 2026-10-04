volatile uint32_t lastTrigger = 0;
volatile uint32_t lastTrigger2 = 0;

volatile uint8_t keyBuffer[16];
volatile uint8_t writePos = 0;
volatile uint8_t readPos  = 0;

inline uint8_t readKeys() {
    uint8_t d = PIND;
    uint8_t c = PINC;
    uint8_t e = PINE;
    uint8_t b = PINB;

    return (b & 0x70)
         | (d & 0x01)
         | ((d & 0x10) >> 3)
         | ((c & 0x40) >> 4)
         | ((d & 0x80) >> 4)
         | ((e & 0x40) >> 2);
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
  // pin 3 bis 10
  
  
  // todo hier Puffer abarbeiten und senden

  //ist alles pullup also falschrum? vielciht einmal byte konverten

  // kann der andere eigetnlich taste gedrückt halten wie wwwwwwwwwwwwwwww? ne kann er nicht wäre auch schlecht auf dem controller der ist für tasten eingabe
}
