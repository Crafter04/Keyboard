void setup()
{
    Serial.begin(115200);

    pinMode(1, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);
    pinMode(3,  INPUT_PULLUP);
    pinMode(4,  INPUT_PULLUP);
    pinMode(5,  INPUT_PULLUP);
    pinMode(6,  INPUT_PULLUP);
    pinMode(7,  INPUT_PULLUP);
    pinMode(8,  INPUT_PULLUP);
    pinMode(9,  INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);

    Serial.println("Tastentest start");
}

void loop()
{
    for (uint8_t pin = 1; pin <= 10; pin++)
    {
        if (digitalRead(pin) == LOW)
        {
            Serial.print("Taste D");
            Serial.print(pin);
            Serial.println(" gedrückt");

            delay(100);
        }
    }
}