volatile uint32_t lastTrigger = 0;

volatile uint8_t keyBuffer[16];
volatile uint8_t writePos = 0;
volatile uint8_t readPos  = 0;

uint8_t readKeys() {
    uint8_t result = 0;

    for (uint8_t i = 0; i < 8; i++) {
        if (digitalRead(3 + i) == HIGH) {
            result |= (1 << i);
        }
    }

    return result;
}

inline void captureKeys() {
    uint8_t next = (writePos + 1) % 16;
    if (next != readPos) {
        keyBuffer[writePos] = readKeys();
        writePos = next;
    }
}

void pressed() {
    uint32_t now = micros();

    if (now - lastTrigger >= 10000) {// 10 ms Sperrzeit
        lastTrigger = now;
        captureKeys();
    }
}
void setup() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), pressed, FALLING);
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

void loop() {
  // todo Puffer abarbeiten und senden
}