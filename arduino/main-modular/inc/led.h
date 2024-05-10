/**
 * @file led.h
 * Header file for LEDs for NRM Navigation
*/

#ifndef __NRM_LED_H__
#define __NRM_LED_H__

/* === Includes ============================================================ */

#include <Arduino.h>
#include <FastLED.h>

/* === Defines ============================================================= */

#define LED_PIN     27
#define NUM_LEDS    9

/* === Typedefs ============================================================ */

typedef enum {
  DANGER = 1500,
  WARNING = 3000
} thresholds_e;

typedef struct {
  int start;
  int end;
} LED_Position_t;

/* === Public Variables ==================================================== */

extern CRGB leds[NUM_LEDS];
// const struct CRGB LED_Colour[2] = {struct CRGB(255, 0, 0), CRGB(255, 165, 0)};

// For demo, only define two sets of LEDs
const LED_Position_t LED_Position_NE = {0, 4};
const LED_Position_t LED_Position_NW = {4, 8};
const LED_Position_t LED_Positions[2] = {LED_Position_NE, LED_Position_NW};

/* === Public Functions ==================================================== */

void LED_init(void);
void LED_processing(int TOF_ID, int* position_arr);

#endif // __NRM_LED_H__
