/**
 * @brief Test suite Integration and Unit Tests
 */

/* === Includes ============================================================ */

// External Libraries
#include <M5Core2.h>
#include <FastLED.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

#include <WiFi.h>
#include <PubSubClient.h>

#include <stdint.h> // uint8_t
#include <Arduino.h>

// Custom Libraries
#include "led.h"


// List of features of the AUnit library that we want to examine.
#define FEATURE_BASELINE 0
#define FEATURE_AUNIT 1
#define FEATURE_AUNIT_VERBOSE 2

int ledDiagnosticArray[6] = {LED_DANGER - 100, LED_DANGER - 100, LED_WARNING + 100, LED_WARNING + 100, LED_OK + 100, LED_OK + 100};

/* === Functions =========================================================== */

void setup() {

//   #if ! defined(EPOXY_DUINO)
//   delay(1000); // wait for stability on some boards to prevent garbage Serial
// #endif
//   Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
//   while(!Serial); // for the Arduino Leonardo/Micro only
// #if defined(EPOXY_DUINO)
//   Serial.setLineModeUnix();
// #endif
  LED_init();
  LED_processing(0, ledDiagnosticArray);
}

void loop() {
  // aunit::TestRunner::run();
}
