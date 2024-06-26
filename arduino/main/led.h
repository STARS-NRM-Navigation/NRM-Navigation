/**
 ******************************************************************************
 * @file led.cpp
 * @author Rachel Chiong
 * @brief LED Header file
 ******************************************************************************
 */

#ifndef __NRM_LED_H__
#define __NRM_LED_H__

/* === Includes ============================================================ */
#ifdef ARDUINO_M5STACK_Core2
#include <M5Core2.h>
#else
#include <Arduino.h>
#endif

#include <FastLED.h>

/* === Defines ============================================================= */

#define LED_PIN     27
#define NUM_LEDS    9

/* === Typedefs ============================================================ */

typedef enum {
  DANGER = 1000,
  WARNING = 1500,
  GOOD = 2000
} thresholds_e;

typedef enum {
  RED_v = 255,
  GREEN_v = 165,
  BLUE_v = 0
} RGB_values;

typedef struct {
  int start;
  int end;
} LED_Position_t;

/* === Public Variables ==================================================== */

extern CRGB leds[NUM_LEDS];
// const struct CRGB LED_Colour[2] = {struct CRGB(255, 0, 0), CRGB(255, 165, 0)};

// For demo, only define two sets of LEDs
const LED_Position_t LED_Position_NE = {0, 3};
const LED_Position_t LED_Position_NW = {4, 7};
const LED_Position_t LED_Position_NW_S = {8, 10};

const LED_Position_t LED_Positions[3] = {LED_Position_NE, LED_Position_NW, LED_Position_NW_S};

/* === Public Functions ==================================================== */

void LED_init(void);
void LED_processing(int TOF_ID, int* position_arr);

#endif // __NRM_LED_H__
