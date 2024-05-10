/**
 ******************************************************************************
 * @file tof.c
 * @author Rachel Chiong
 * @brief TOF source file
 ******************************************************************************
 */

#include "../inc/tof.h"

/* === Global Variables ==================================================== */

// Store Imagers in an array
TOF_Imager_t TOF_Imagers[8];

SparkFun_VL53L5CX testImager;
VL53L5CX_ResultsData measurementData;

int imageResolution; //Used to pretty print output
int imageWidth; //Used to pretty print output

/* === Functions =========================================================== */

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

void TOF_scan(int TOF_ID) {
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

void TOF_Grid_Processing(int TOF_ID, int* new_aggregate_columns) {
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