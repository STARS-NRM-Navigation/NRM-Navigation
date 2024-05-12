/**
 *
*/

#include "led.h"

/* === Public Variables ==================================================== */

CRGB leds[NUM_LEDS];

void LED_init(void) {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clearData();
  Serial.println("LEDs successfully initialised.");
}

void LED_processing(int TOF_ID, int* position_arr) {
  for (int i = 0; i < 4; i++) {
    if (position_arr[i] < DANGER) {

      leds[LED_Position_NE.start + i] = CRGB(255, 0, 0);
      FastLED.show();
    } else if (position_arr[i] < WARNING) {

      leds[LED_Position_NE.start + i] = CRGB(255, 165, 0);
      FastLED.show();
    } else {

      leds[LED_Position_NE.start + i] = CRGB(0, 0, 0);
      FastLED.show();
    }
  }
}
