int leds[] = {3, 5, 6, 9, 10};
int numLeds = 5;

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  for (int brightness = 0; brightness <= 255; brightness++) {
    for (int i = 0; i < numLeds; i++) {
      analogWrite(leds[i], brightness);
    }

    delay(5);
  }
  for (int brightness = 255; brightness >= 0; brightness--) {

    for (int i = 0; i < numLeds; i++) {
      analogWrite(leds[i], brightness);
    }

    delay(5);
  }
}
