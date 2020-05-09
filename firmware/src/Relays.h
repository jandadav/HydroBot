#ifndef RELAYS_h
#define RELAYS_h

#include <Arduino.h>

#define PIN_PUMP D1


class Relays {
    public:
        void setup();
        void pumpOn();
        void pumpOff();
        int getPumpState();
};

void Relays::setup() {
    pinMode(PIN_PUMP, OUTPUT);
    pumpOff();
}

void Relays::pumpOn() {
    digitalWrite(PIN_PUMP, HIGH);
}

void Relays::pumpOff() {
    digitalWrite(PIN_PUMP, LOW);
}

int Relays::getPumpState() {
    return digitalRead(PIN_PUMP);
}


#endif