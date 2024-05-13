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

/* === Main ================================================================ */

char lcd_buffer[80];

void setup() {
  M5.begin();
  Serial.begin(115200);
  delay(1000);
  Serial.println("NRM_Navigation FW v0.0.1");
  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  M5.Lcd.println("LCD on");

  LED_init();
  TOF_init();
}

void loop() {

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
}