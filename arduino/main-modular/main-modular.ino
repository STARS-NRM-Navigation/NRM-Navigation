/**
 * @file main.ino
 *
 * Source code for the NRM_Navigation solution
 * @author BIOE6901 NRM_Navigation
 */

/* === Includes ============================================================ */

// External Libraries
#include <FastLED.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

// Custom Libraries
#include "inc/tof.h"
#include "inc/led.h"

/* === Main ================================================================ */

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("SparkFun VL53L5CX Imager Example");
  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");

  LED_init();
  TOF_init();
}

void loop() {

  // Run TOF scan
  TOF_scan(TOF_NE);
  int grid[4];
  TOF_Grid_Processing(TOF_NE, grid);
  printf("Processed Grid: %d, %d, %d, %d\n", grid[0], grid[1], grid[2], grid[3]);

  // Turn appropriate LEDs on
  LED_processing(TOF_NE, grid);
  delay(50); //Small delay between polling
}