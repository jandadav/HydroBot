#ifndef DISTANCE_h
#define DISTANCE_h

#include <Arduino.h>

const int trigPin = D5;
const int echoPin = D6;
long duration;
int distanceCm;

class Distance {
    public:
        void setup();
        int getDistanceCm();
};

void Distance::setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

int Distance::getDistanceCm() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    return duration*0.034/2;
}


#endif