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
int WarningThreshold = WARNING;
int OKThreshold = GOOD;

/* === Functions =========================================================== */

void LED_init(void) {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clearData();
  Serial.println("LEDs successfully initialised.");
}

int* gradient_processing(int distance) {
  // Initialise variables
  int red;
  int green;
  // Calculate RGB values
  if (distance > WARNING) {
    double factor = (double)(distance-WARNING)/(GOOD - WARNING);
    red = (int)(RED_v * (1 - factor));
    green = (int)(GREEN_v);
  } else if (distance <= WARNING) {
    double factor = (double)(distance-DANGER)/(WARNING - DANGER);
    red = (int)(RED_v);
    green = (int)(RED_v * factor);
  }
  // Assign RGB values to the array
  int RGBvalues[3] = {red, green, BLUE_v};
  // RGBvalues[0] = red;
  // RGBvalues[1] = green;
  // RGBvalues[2] = BLUE;
  return RGBvalues;
}

void LED_processing(int TOF_ID, int* position_arr) {
  int* RGBvalues;
  for (int i = 0; i < 8; i++) {
    if (position_arr[i] < DangerThreshold) {

      leds[LED_Positions[TOF_ID].start + i] = CRGB(255, 0, 0);
      FastLED.show();
    } else if (position_arr[i] > OKThreshold) {

      leds[LED_Positions[TOF_ID].start + i] = CRGB(255, 165, 0);
      FastLED.show();
    } else {
      RGBvalues = gradient_processing(position_arr[i]);
      leds[LED_Positions[TOF_ID].start + i] = CRGB(RGBvalues[0], RGBvalues[1], RGBvalues[2]);
      FastLED.show();
    }
  }
}
