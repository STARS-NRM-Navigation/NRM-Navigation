/**
 * @file main.ino
 * 
 * Source code for the NRM_Navigation solution
 * @author BIOE6901 NRM_Navigation
 */

/* === Includes ============================================================ */

#include <FastLED.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX
// TODO: for diagnostic purposes, add MQTT and graphs (test suite)
/* === Defines ============================================================= */

/* LEDs */
#define LED_PIN     27
#define NUM_LEDS    9

/* TOFs */
#define TOF_NUM 8
#define TOF_RESOLUTION 64 // 8 x 8 array
#define TOF_IMG_WIDTH 8

#define TOF_NE_ADDR 0x52
#define TOF_NW_ADDR 0x52
// Additional TOF addresses will be defined here...

/* === Global Variables ==================================================== */

/* LEDs */
CRGB leds[NUM_LEDS];

typedef enum {
  DANGER = 1500,
  WARNING = 3000
} thresholds_e;

const CRGB LED_Colour[2] = {CRGB(255, 0, 0), CRGB(255, 165, 0)};

typedef struct {
  int start;
  int end;
} LED_Position_t;

// For demo, only define two sets of LEDs
const LED_Position_t LED_Position_NE = {0, 4};
const LED_Position_t LED_Position_NW = {4, 8};
const LED_Position_t LED_Positions[2] = {LED_Position_NE, LED_Position_NW};

/* TOFs */

// 
typedef enum {
  TOF_NE,
  TOF_NW,
  TOF_SW,
  TOF_SE,
  TOF_NE_S,
  TOF_NW_S,
  TOF_SW_S,
  TOF_SE_S
} TOF_Position_e;

typedef struct {
 TOF_Position_e TOF_ID;
 uint8_t i2c_address;
 SparkFun_VL53L5CX imager;
 VL53L5CX_ResultsData measurementData;
} TOF_Imager_t;

// Store Imagers in an array
TOF_Imager_t TOF_Imagers[8];

SparkFun_VL53L5CX testImager;
VL53L5CX_ResultsData measurementData;

int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output

/* === LEDs Functions ====================================================== */

void LED_init(void) {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clearData();
  Serial.println("LEDs successfully initialised.");
}

void LED_processing(TOF_Position_e TOF_ID, int* position_arr) {
  for (int i = 0; i < 4; i++) {
    if (position_arr[i] < DANGER) {

      leds[LED_Position_NE.start + i] = CRGB(255, 0, 0);
      FastLED.show();
    } else if (position_arr[i] < WARNING) {

      leds[LED_Position_NE.start + i] = CRGB(255, 165, 0);
      FastLED.show();
    } else {

      leds[LED_Position_NE.start + i] = CRGB(0, 0, 0);
      FastLED.show();
    }
  }
}

/* === TOF Functions ======================================================= */

void TOF_init(void) {
  TOF_Imagers[TOF_NE].TOF_ID = TOF_NE;
  TOF_Imagers[TOF_NE].i2c_address = 0x52;

  SparkFun_VL53L5CX imager;
  VL53L5CX_ResultsData measurement;
  TOF_Imagers[TOF_NE].measurementData = measurement;
  TOF_Imagers[TOF_NE].imager = imager;

  Serial.printf("Initialising TOF: %d Addr: %2X\n", TOF_Imagers[TOF_NE].TOF_ID, TOF_Imagers[TOF_NE].i2c_address);
  
  Wire.begin(); //This resets to 100kHz I2C
  Wire.setClock(400000); //Sensor has max I2C freq of 400kHz

  Serial.println("Successfully configured clock");
  // TOF_Imagers[TOF_NE].imager.setAddress(TOF_Imagers[TOF_NE].i2c_address);

  if (TOF_Imagers[TOF_NE].imager.begin() == false) {
    Serial.printf("[TOF] %d: Not found at address: %2X\n", TOF_NE, TOF_Imagers[TOF_NE].i2c_address);
    while (1) ;
  }

  TOF_Imagers[TOF_NE].imager.setResolution(TOF_RESOLUTION);
  TOF_Imagers[TOF_NE].imager.startRanging();

  Serial.println("Successfully initialised TOF");
}

void TOF_scan(TOF_Position_e TOF_ID) {
  if (TOF_Imagers[TOF_ID].imager.isDataReady() == true) {
    // Read distance data into array
    if (TOF_Imagers[TOF_ID].imager.getRangingData(&TOF_Imagers[TOF_ID].measurementData)) {
      //The ST library returns the data transposed from zone mapping shown in datasheet
      // Pretty-print data with increasing y, decreasing x to reflect reality
      for (int y = 0 ; y < TOF_RESOLUTION ; y += TOF_IMG_WIDTH) {
        for (int x = TOF_IMG_WIDTH - 1 ; x >= 0; x--) {
          Serial.print("\t");
          Serial.print(TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y]);
        }
        Serial.println();
      }
      Serial.println();
    }
  }
}

void TOF_Grid_Processing(TOF_Position_e TOF_ID, int* new_aggregate_columns) {
  // Get the data
  VL53L5CX_ResultsData data = TOF_Imagers[TOF_ID].measurementData;
  // get average per two columns
  int aggregate_columns[TOF_IMG_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
  int column = 0;

  for (int y = 0 ; y < TOF_RESOLUTION ; y += TOF_IMG_WIDTH) {
      for (int x = TOF_IMG_WIDTH - 1 ; x >= 0; x--) {
        aggregate_columns[x] = aggregate_columns[x] + TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y];
      }
  }

  for (int i = 0; i < TOF_IMG_WIDTH; i += 2) {
    int result = (aggregate_columns[i] + aggregate_columns[i+1])/(TOF_IMG_WIDTH * 2);
    new_aggregate_columns[i/2] = result; 
  }
}

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