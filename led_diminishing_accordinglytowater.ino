#include <HX711.h>
#include <ESP32Servo.h>

// ==============================
// SERVO
// ==============================

#define SERVO_PIN 26

Servo butterflyServo;

int angle = 60;
int direction = 10;

unsigned long servoTimer = 0;

// Fastest and slowest servo movement
const int FAST_SERVO_INTERVAL = 4;
const int SLOW_SERVO_INTERVAL = 100;


// ==============================
// HX711 LOAD CELL
// ==============================

#define HX711_DT  32
#define HX711_SCK 33

HX711 scale;

float calibrationFactor = -774.94;


// ==============================
// LEDs
// ==============================

const int LED_PINS[] = {
  4, 5, 13, 14, 16, 17, 18, 19
};

const int NUM_LEDS = 8;


// ==============================
// BOTTLE SETTINGS
// ==============================

const float BOTTLE_DETECT_G = 50.0;
const float BOTTLE_LIFT_G = 40.0;

const unsigned long WAIT_TIME = 20000;

const float FADE_SPEED = 5;


// ==============================
// VARIABLES
// ==============================

float bottleWeight = 0.0;

float currentBrightness = 0.0;

unsigned long fadeTimerStart = 0;

bool bottlePresent = false;

bool fading = false;


// ==============================
// SETUP
// ==============================

void setup() {

  Serial.begin(115200);

  delay(1000);


  // ============================
  // SERVO SETUP
  // ============================

  butterflyServo.setPeriodHertz(50);

  butterflyServo.attach(
    SERVO_PIN,
    500,
    2400
  );

  butterflyServo.write(angle);


  // ============================
  // HX711 SETUP
  // ============================

  scale.begin(
    HX711_DT,
    HX711_SCK
  );

  scale.set_scale(
    calibrationFactor
  );


  Serial.println();
  Serial.println("==============================");
  Serial.println("   BUTTERFLY + WATER SYSTEM");
  Serial.println("==============================");
  Serial.println();

  Serial.println("Taring load cell...");

  delay(2000);

  scale.tare();

  Serial.println("Tare complete.");
  Serial.println();

  Serial.println("Put bottle on the load cell.");
  Serial.println();


  // ============================
  // LED SETUP
  // ============================

  for (
    int i = 0;
    i < NUM_LEDS;
    i++
  ) {

    ledcAttach(
      LED_PINS[i],
      5000,
      8
    );

    ledcWrite(
      LED_PINS[i],
      0
    );
  }
}


// ==============================
// MAIN LOOP
// ==============================

void loop() {

  // ============================
  // BUTTERFLY
  // ============================

  updateButterfly();


  // ============================
  // CHECK HX711
  // ============================

  if (!scale.is_ready()) {

    Serial.println(
      "HX711 not ready!"
    );

    delay(500);

    return;
  }


  float weight =
    scale.get_units(5);


  // ============================
  // NO BOTTLE
  // ============================

  if (!bottlePresent) {

    if (
      weight > BOTTLE_DETECT_G
    ) {

      bottleWeight =
        weight;

      bottlePresent =
        true;

      fading =
        false;

      currentBrightness =
        255;

      fadeTimerStart =
        millis();

      setLEDs(255);


      Serial.println();
      Serial.println(
        ">>> BOTTLE DETECTED"
      );

      Serial.print(
        "Bottle weight: "
      );

      Serial.print(
        bottleWeight,
        1
      );

      Serial.println(
        " g"
      );

      Serial.println(
        "LEDs = 100%"
      );

      Serial.println(
        "Butterfly = FAST"
      );

      Serial.println(
        "20 second timer started."
      );

      Serial.println();
    }

    else {

      Serial.print(
        "Waiting... Weight: "
      );

      Serial.print(
        weight,
        1
      );

      Serial.println(
        " g"
      );
    }


    delay(100);

    return;
  }


  // ============================
  // BOTTLE PICKED UP
  // ============================

  if (
    weight <
    bottleWeight - BOTTLE_LIFT_G
  ) {

    bottlePresent =
      false;

    fading =
      false;

    Serial.println();
    Serial.println(
      ">>> BOTTLE PICKED UP"
    );

    Serial.println(
      "LED fading stopped."
    );

    Serial.println(
      "Put bottle back to restart."
    );

    Serial.println();

    delay(500);

    return;
  }


  // ============================
  // TIMER
  // ============================

  unsigned long elapsed =
    millis() - fadeTimerStart;


  // ============================
  // FIRST 20 SECONDS
  // ============================

  if (
    elapsed <
    WAIT_TIME
  ) {

    unsigned long secondsPassed =
      elapsed / 1000;


    Serial.print(
      "Bottle present | "
    );

    Serial.print(
      "Time: "
    );

    Serial.print(
      secondsPassed
    );

    Serial.print(
      " / 20 sec | "
    );

    Serial.println(
      "LED: 100% | Butterfly: FAST"
    );
  }


  // ============================
  // AFTER 20 SECONDS
  // ============================

  else {

    if (!fading) {

      fading =
        true;


      Serial.println();
      Serial.println(
        ">>> 20 SECONDS COMPLETE"
      );

      Serial.println(
        ">>> LEDS DIMMING..."
      );

      Serial.println(
        ">>> BUTTERFLY SLOWING..."
      );

      Serial.println();
    }


    // ==========================
    // DIM LEDS
    // ==========================

    if (
      currentBrightness >
      5
    ) {

      currentBrightness -=
        FADE_SPEED;
    }


    if (
      currentBrightness <
      5
    ) {

      currentBrightness =
        5;
    }


    setLEDs(
      (int)currentBrightness
    );


    // ==========================
    // STATUS
    // ==========================

    Serial.print(
      "Brightness: "
    );

    Serial.print(
      currentBrightness,
      1
    );

    Serial.print(
      " / 255 | Butterfly interval: "
    );

    Serial.print(
      getServoInterval()
    );

    Serial.println(
      " ms"
    );
  }


  delay(100);
}


// ==============================
// BUTTERFLY FUNCTION
// ==============================

void updateButterfly() {

  int interval =
    getServoInterval();


  if (
    millis() - servoTimer >= interval
  ) {

    servoTimer =
      millis();


    butterflyServo.write(
      angle
    );


    angle += direction;


    // Reverse at 150°
    if (
      angle >= 150
    ) {

      angle = 150;

      direction = -1;
    }


    // Reverse at 30°
    if (
      angle <= 30
    ) {

      angle = 30;

      direction = 1;
    }
  }
}


// ==============================
// SERVO SPEED
// ==============================

int getServoInterval() {

  /*
     Brightness = 255
       -> 15 ms interval
       -> FAST

     Brightness = 128
       -> ~82 ms interval
       -> MEDIUM

     Brightness = 5
       -> 150 ms interval
       -> SLOW
  */

  int brightness =
    constrain(
      (int)currentBrightness,
      5,
      255
    );


  int interval =
    map(
      brightness,
      5,
      255,
      SLOW_SERVO_INTERVAL,
      FAST_SERVO_INTERVAL
    );


  return interval;
}


// ==============================
// LED FUNCTION
// ==============================

void setLEDs(
  int brightness
) {

  brightness =
    constrain(
      brightness,
      0,
      255
    );


  for (
    int i = 0;
    i < NUM_LEDS;
    i++
  ) {

    ledcWrite(
      LED_PINS[i],
      brightness
    );
  }
}
