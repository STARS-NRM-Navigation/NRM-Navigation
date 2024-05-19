/**
 * @file main.ino
 *
 * Source code for the NRM_Navigation solution
 * @author BIOE6901 NRM_Navigation
 */

/* === Includes ============================================================ */

// External Libraries

#ifdef ARDUINO_M5STACK_Core2
#include <M5Core2.h>
#else
#include <Arduino.h>
#endif

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

// StaticJsonDocument<1024> doc;

// Create the root array
// JsonArray root = doc.to<JsonArray>();

/* === Helper Functions ==================================================== */

// void create_object(int id, int* gridArray) {
//   JsonObject obj = root.createNestedObject();
//   obj["id"] = id;
//   JsonArray grid = doc.createNestedArray("grid");
//   for (int i = 0; i < GRID_SIZE; i++) {
//     grid.add(test[i]);  // Add elements from the test array to the grid array
//   }
// }

#ifdef ARDUINO_M5STACK_Core2
void print_lcd(char* buffer) {
  ++value;
  M5.Lcd.println(buffer);
  if (value % 12 == 0) {
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0);
  }
}
#endif

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
#ifdef ARDUINO_M5STACK_Core2
  M5.begin();
#endif
  Serial.begin(115200);
  delay(1000);
  Serial.println("----------------- NRM_Navigation FW v0.1.0 ---------------------------");
  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");

#ifdef ARDUINO_M5STACK_Core2
  M5.Lcd.println("LCD on");
#endif

  LED_init();
  TOF_init();

#endif
}

void loop() {

#ifdef UNITTEST
  Test::run();
#else

  // Run TOF scan
  for (int i = 0; i < 8; i++) {
    if (TOF_Imagers[i].active) {
      TOF_scan(i);
      TOF_Grid_Processing(i, grid);
#ifdef ARDUINO_M5STACK_Core2
      sprintf(lcd_buffer, "Processed Grid [%d]: %d, %d, %d, %d\n", i, grid[0], grid[1], grid[2], grid[3]);
      // printf("Processed Grid [%d]: %d, %d, %d, %d\n", i, grid[0], grid[1], grid[2], grid[3]);
      print_lcd(lcd_buffer);
#endif
      // Turn appropriate LEDs on
      LED_processing(i, grid);
    } else {
      // printf("[%d] SENSOR NOT ACTIVE\n", i);
    }
  }

  delay(100); //Small delay between polling
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
