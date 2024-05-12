/**
 ******************************************************************************
 * @file tof.cpp
 * @author Rachel Chiong
 * @brief TOF source file
 ******************************************************************************
 */

#include "tof.h"

/* === Global Variables ==================================================== */

// Store Imagers in an array
TOF_Imager_t TOF_Imagers[8];

int imageResolution; //Used to pretty print output
int imageWidth; //Used to pretty print output

/* === Functions =========================================================== */

void TOF_MUX_Select(int TOF_ID) {
  if (TOF_ID > 7) return;
 
  Wire.beginTransmission(I2C_MUX_ADDR);
  Wire.write(1 << TOF_ID);
  Wire.endTransmission();
}

void TOF_config(int TOF_ID) {

  TOF_Imagers[TOF_ID].TOF_ID = (TOF_Position_e) TOF_ID;
  SparkFun_VL53L5CX imager;
  VL53L5CX_ResultsData measurement;
  TOF_Imagers[TOF_ID].measurementData = measurement;
  TOF_Imagers[TOF_ID].imager = imager;

  // Select the TOF to use from the MUX
  TOF_MUX_Select(TOF_ID);

  if (TOF_Imagers[TOF_ID].imager.begin() == false) {
    Serial.printf("[TOF] %d: Not found at address: %2X\n", TOF_NE, TOF_Imagers[TOF_NE].i2c_address);
    while (1) ;
  } else {
    Serial.printf("Initialising TOF: %d Addr: %2X\n", TOF_Imagers[TOF_NE].TOF_ID, TOF_Imagers[TOF_NE].imager.getAddress());
  }

  TOF_Imagers[TOF_ID].imager.setResolution(TOF_RESOLUTION);
  TOF_Imagers[TOF_ID].imager.startRanging();
}

void TOF_init(void) {

  Wire.begin(); //This resets to 100kHz I2C
  Wire.setClock(400000); //Sensor has max I2C freq of 400kHz

  Serial.println("Successfully configured clock");
  for (int i = 0; i < TOF_NUM_TEST; i++) {
    TOF_config(i);
  }
  Serial.printf("Successfully initialised %d TOFs.", TOF_NUM_TEST);
}

void TOF_scan(int TOF_ID) {

  TOF_MUX_Select(TOF_ID);

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
  // Get data
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

void TOF_Grid_Processing_min(int TOF_ID, int* aggregate_columns) {
 // Get data
 VL53L5CX_ResultsData data = TOF_Imagers[TOF_ID].measurementData;
 int aggregate_columns_2[TOF_IMG_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
 int column = 0;
 // Create array of values and find minimum in each column
 for (int y = 0; y < TOF_RESOLUTION; y += TOF_IMG_WIDTH) {
  int min = TOF_Imagers[TOF_ID].measurementData.distance_mm[(TOF_IMG_WIDTH-1) + y];
  for (int x = TOF_IMG_WIDTH - 1; x >= 0; x--) {
    if (TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y] < min) {
      min = TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y];
    }
  }
  aggregate_columns_2[y] = min;
 }
}