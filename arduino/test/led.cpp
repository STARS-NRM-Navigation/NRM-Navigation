/**
 ******************************************************************************
 * @file led.cpp
 * @author Rachel Chiong
 * @brief LED source file
 ******************************************************************************
 */

#include "led.h"

/* === Public Variables ==================================================== */

CRGB leds[NUM_LEDS];

int DangerThreshold = LED_DANGER;
int WarningThreshold = LED_WARNING;

/* === Functions =========================================================== */

void LED_init(void) {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clearData();
  Serial.println("LEDs successfully initialised.");
}

void LED_processing(int TOF_ID, int* position_arr) {
  int length = sizeof(position_arr);
  for (int i = 0; i < 6; i++) {
    if (position_arr[i] < DangerThreshold) {

      leds[LED_Positions[TOF_ID].start + i] = CRGB(255, 0, 0);
      FastLED.show();
    } else if (position_arr[i] < WarningThreshold) {

      leds[LED_Positions[TOF_ID].start + i] = CRGB(255, 165, 0);
      FastLED.show();

    } else {

      leds[LED_Positions[TOF_ID].start + i] = CRGB(0, 255, 0);
      FastLED.show();
    }
  }
}
