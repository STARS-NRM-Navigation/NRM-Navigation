/**
 * @file main.ino
 *
 * Source code for the NRM_Navigation solution
 * @author BIOE6901 NRM_Navigation
 */

/* === Includes ============================================================ */

// External Libraries
#include <M5Core2.h>
#include <FastLED.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

// Custom Libraries
#include "tof.h"
#include "led.h"

#include <ArduinoUnit.h>

/* === Global Variables ==================================================== */
int value = 0;
int grid[8];

/* === Helper Functions ==================================================== */

void print_lcd(char* buffer) {
  ++value;
  M5.Lcd.println(buffer);
  if (value % 12 == 0) {
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0);
  }
}
// #define UNITTEST
/* === Unit Test Prototypes ================================================ */
void LED_Processing_Danger(void);
void LED_Processing_Warning(void);
void LED_Processing_Mixed(void);

/* === Main ================================================================ */

char lcd_buffer[80];

void setup() {
#ifdef UNITTEST
  Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
  Serial.println("------------UNIT TESTING ----------------------");
#else

  M5.begin();
  Serial.begin(115200);
  delay(1000);
  Serial.println("NRM_Navigation FW v0.0.1");
  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  M5.Lcd.println("LCD on");

  LED_init();
  TOF_init();
  
#endif
}

void loop() {

#ifdef UNITTEST
  Test::run();
#else

  // Run TOF scan
  for (int i = 0; i < 1; i++) {
    TOF_scan(i);
    TOF_Grid_Processing(TOF_NE, grid);
    sprintf(lcd_buffer, "Processed Grid [%d]: %d, %d, %d, %d\n", i, grid[0], grid[1], grid[2], grid[3]);
    printf("Processed Grid [%d]: %d, %d, %d, %d\n", i, grid[0], grid[1], grid[2], grid[3]);
    print_lcd(lcd_buffer);
    // Turn appropriate LEDs on
    LED_processing(i, grid);
  }

  delay(50); //Small delay between polling
#endif
}

/* === Unit Tests ============================================================ */

void LED_Processing_Danger(void) {
  int TOF_ID = 1;
  int position_arr[4] = {100, 300, 500, 700};
  LED_processing(TOF_ID, position_arr);
  
  assertEqual(leds[LED_Positions[TOF_ID].start + 0].raw[0], CRGB(255, 0, 0).raw[0]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 1].raw[0], CRGB(255, 0, 0).raw[0]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 2].raw[0], CRGB(255, 0, 0).raw[0]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 3].raw[0], CRGB(255, 0, 0).raw[0]);
}

void LED_Processing_Warning(void) {
  int TOF_ID = 1;
  int position_arr[4] = {1501, 2000, 1977, 1600};
  LED_processing(TOF_ID, position_arr);
  
  assertEqual(leds[LED_Positions[TOF_ID].start + 0].raw[1], CRGB(255, 165, 0).raw[1]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 1].raw[1], CRGB(255, 165, 0).raw[1]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 2].raw[1], CRGB(255, 165, 0).raw[1]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 3].raw[1], CRGB(255, 165, 0).raw[1]);
}

void LED_Processing_Mixed(void) {
  int TOF_ID = 1;
  int position_arr[4] = {1501, 2000, 200, 800};
  LED_processing(TOF_ID, position_arr);
  
  assertEqual(leds[LED_Positions[TOF_ID].start + 0].raw[1], CRGB(255, 165, 0).raw[1]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 1].raw[1], CRGB(255, 165, 0).raw[1]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 2].raw[0], CRGB(255, 0, 0).raw[0]);
  assertEqual(leds[LED_Positions[TOF_ID].start + 3].raw[0], CRGB(255, 0, 0).raw[0]);

}

void TOF_Config_Test(void) {
  
}
