#include "FastLED.h"

// How many leds are in the strip?
#define NUM_LEDS 9

// Data pin that led data will be written out over
#define DATA_PIN 17

#define FADE_RATE (4)

CRGB leds[NUM_LEDS];

int32_t pos = NUM_LEDS - 1;
int32_t pos_inc = -1;

typedef enum {
  MODE_FLUXING = 0,
  MODE_RED = 1,
  MODE_GREEN = 2,
  MODE_BLUE = 3,
  MODE_RANDOM = 4,
  MODE_RANDOM_HOLD = 5,
  MODE_DONE
} modes_t;

void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(2000);
    //Serial.begin(38400);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    randomSeed(analogRead(0));
    pinMode(19, INPUT_PULLUP);
}

int val = 0;
int timeDelay = 50;
mode_t mode = MODE_FLUXING;
int oldButtonVal = 1;
uint8_t rndColors[3] = {0, 0, 0};

void loop() {
  for(uint32_t led = 0; led < NUM_LEDS; led++) {
    int fadeRate = val >> 2;
    if(leds[led].r >= fadeRate) {
      leds[led].r -= fadeRate;
    } else {
      leds[led].r = 0;
    }

    if(leds[led].g >= fadeRate) {
      leds[led].g -= fadeRate;
    } else {
      leds[led].g = 0;
    }

    if(leds[led].b >= fadeRate) {
      leds[led].b -= fadeRate;
    } else {
      leds[led].b = 0;
    }
  }

  FastLED.show();

  val = analogRead(6) >> 2;
  timeDelay = analogRead(7) >> 4;

  switch(mode) {
    case MODE_FLUXING: {
      leds[pos].r = val;
      leds[pos].g = val;
      leds[pos].b = val;
      break;
    }

    case MODE_RED: {
      leds[pos].r = val;
      break;
    }

    case MODE_GREEN: {
      leds[pos].g = val;
      break;
    }

    case MODE_BLUE: {
      leds[pos].b = val;
      break;
    }

    case MODE_RANDOM: {
      if(pos == (NUM_LEDS - 1)) {
        int maxVal = (val * 15)/10;
        if(maxVal > 255) {
          maxVal = 255;
        }
        rndColors[0] = random(0, val);
        rndColors[1] = random(0, val - rndColors[0]);
        rndColors[2] = val - rndColors[0] - rndColors[1];
      }
      
      // Not breaking here on purpose!
    }

    case MODE_RANDOM_HOLD: {
      leds[pos].r = rndColors[0];
      leds[pos].g = rndColors[1];
      leds[pos].b = rndColors[2];
      break;
    }

    default: {
      mode = MODE_FLUXING;
      break;
    }
  }
  
  pos += pos_inc;

  if(pos < 0) {
    pos = NUM_LEDS - 1;
  }
  
  int buttonVal = digitalRead(19);
  
  if((buttonVal == 0) && (oldButtonVal == 1)) {
    mode++;
    if(mode >= MODE_DONE) {
      mode = MODE_FLUXING;
    }

    if(mode == MODE_RANDOM) {
      rndColors[0] = val/2;
      rndColors[1] = val/2;
      rndColors[2] = val/2;
    }
    delay(10);
  }
  
  oldButtonVal = buttonVal;

  delay(timeDelay);
}
