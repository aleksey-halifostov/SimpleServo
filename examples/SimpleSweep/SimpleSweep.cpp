#include <Arduino.h>
#include <SimpleServo.h>

SimpleServo servo; // Instantiate the servo driver

void setup() {

    servo.attach(5); // // Attach the servo to digital pin 5.
}

void loop() {

    // Sweep from 0 to 180 degrees
    for (int i = 0; i <= 180; i++) {

        servo.set_angle(i);
        delay(20);
    }

    // Sweep from 180 to 0 degrees
    for (int i = 180; i >=0; i--) {

        servo.set_angle(i);
        delay(20);
    }
}