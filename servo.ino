#include <Servo.h>

Servo butterfly;

void setup() {
  butterfly.attach(9);
}

void loop() {
  // Wing goes one direction
  butterfly.write(60);
  delay(500);

  // Wing goes the other direction
  butterfly.write(120);
  delay(300);
}