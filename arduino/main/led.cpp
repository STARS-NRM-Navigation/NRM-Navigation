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

int DangerThreshold = DANGER;
int Orange = (DANGER+WARNING)/2;
int WarningThreshold = WARNING;
int Lime = (WARNING+GOOD)/2;
int OKThreshold = GOOD;

/* === Functions =========================================================== */

void LED_init(void) {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clearData();
  Serial.println("LEDs successfully initialised.");
}



void LED_processing(int TOF_ID, int* position_arr) {
//int* RGBvalues;
  for (int i = 0; i < 8; i++) {
    if (position_arr[i] < DangerThreshold) {

      leds[LED_Positions[TOF_ID].start + i] = CRGB(255, 0, 0);
      FastLED.show();

    } else if (position_arr[i] < Orange) {
      leds[LED_Positions[TOF_ID].start + i] = CRGB(255,140,0);
      FastLED.show();
    } else if (position_arr[i] < WarningThreshold) {

      leds[LED_Positions[TOF_ID].start + i] = CRGB(255,255,0);
      FastLED.show();
    } else {	
      leds[LED_Positions[TOF_ID].start + i] = CRGB(0, 165, 0);
      FastLED.show();
    }
  }
}
