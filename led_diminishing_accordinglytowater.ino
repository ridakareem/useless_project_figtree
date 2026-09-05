#include <HX711.h>



#define HX711_DT  32
#define HX711_SCK 33

const int LED_PINS[] = {
  4, 5, 13, 14, 16, 17, 18, 19
};

const int NUM_LEDS = 8;



HX711 scale;

float calibrationFactor = -774.94;





const float BOTTLE_DETECT_G = 50.0;


const float BOTTLE_LIFT_G = 40.0;

const unsigned long WAIT_TIME = 20000;


const float FADE_SPEED = 5;




float bottleWeight = 0.0;

float currentBrightness = 0.0;

unsigned long fadeTimerStart = 0;

bool bottlePresent = false;

bool fading = false;




void setup() {

  Serial.begin(115200);

  delay(1000);


  scale.begin(
    HX711_DT,
    HX711_SCK
  );

  scale.set_scale(
    calibrationFactor
  );


  Serial.println();
  Serial.println("==============================");
  Serial.println("       LED DIMMING TEST");
  Serial.println("==============================");
  Serial.println();

  Serial.println("Taring load cell...");

  delay(2000);

  scale.tare();

  Serial.println("Tare complete.");
  Serial.println();

  Serial.println("Put bottle on the load cell.");
  Serial.println();


 
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



void loop() {



  if (!scale.is_ready()) {

    Serial.println(
      "HX711 not ready!"
    );

    delay(500);

    return;
  }



  float weight =
    scale.get_units(5);




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


    delay(500);

    return;
  }


 

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




  unsigned long elapsed =
    millis() - fadeTimerStart;




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
      "LED: 100%"
    );
  }


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

      Serial.println();
    }


  

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


    Serial.print(
      "Bottle present | Brightness: "
    );

    Serial.print(
      currentBrightness,
      1
    );

    Serial.println(
      " / 255"
    );
  }


  delay(100);
}



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
