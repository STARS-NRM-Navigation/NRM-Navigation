/**
 ******************************************************************************
 * @file tof.cpp
 * @author Rachel Chiong
 * @brief TOF source file
 ******************************************************************************
 */

#include "tof.h"

/* === Global Variables ==================================================== */

#define TOF_RST_PIN 19

// Store Imagers in an array
TOF_Imager_t TOF_Imagers[8];

int imageResolution; //Used to pretty print output
int imageWidth; //Used to pretty print output

int deviceAddress_0 = 0x44; 
int deviceAddress_1 = 0x29; 
int SensorCount;

/* === Functions =========================================================== */

void TOF_config(int TOF_ID, uint8_t i2c_addr, uint8_t rst_pin) {
  // Select the TOF sensor
  if (rst_pin != -1) {
    delay(500);
    digitalWrite(rst_pin, LOW);
    delay(500);
  }

  // Populate TOF struct
  TOF_Imagers[TOF_ID].TOF_ID = (TOF_Position_e) TOF_ID;
  SparkFun_VL53L5CX imager;
  VL53L5CX_ResultsData measurement;
  TOF_Imagers[TOF_ID].measurementData = measurement;
  TOF_Imagers[TOF_ID].imager = imager;
  TOF_Imagers[TOF_ID].active = false;

  Wire.beginTransmission(i2c_addr);
  if (Wire.endTransmission() == 0) {
    // Address is already correct. Do nothing
    TOF_Imagers[TOF_ID].active = true;
  } else {
    // Attempt to set new address
    if (TOF_Imagers[TOF_ID].imager.setAddress(i2c_addr) == false) {
      Serial.printf("[TOF %d] Re-address to %2X Failed. Hanging...\n");
      while (1);
    } else {
      Serial.printf("[TOF %d] Re-address to %2X successful! \n");
      TOF_Imagers[TOF_ID].active = true;
    }
  }

  TOF_Imagers[TOF_ID].i2c_address = i2c_addr;

  if (TOF_Imagers[TOF_ID].imager.begin(i2c_addr) == false) {
    Serial.printf("[TOF] %d: Not found at address: %2X\n", TOF_ID, i2c_addr);
    while (1) ;
  } else {
    Serial.printf("Initialising TOF: %d Addr: %2X\n", TOF_ID, TOF_Imagers[TOF_ID].imager.getAddress());
    TOF_Imagers[TOF_ID].active = true;
  }
  if (TOF_Imagers[TOF_ID].active) {
    TOF_Imagers[TOF_ID].imager.setResolution(TOF_RESOLUTION);
    TOF_Imagers[TOF_ID].imager.startRanging();
  }
}

void TOF_config_raw(void) {
  int TOF_ID = 0;
  TOF_Imagers[TOF_ID].TOF_ID = (TOF_Position_e) TOF_ID;
  SparkFun_VL53L5CX imager;
  VL53L5CX_ResultsData measurement;
  TOF_Imagers[TOF_ID].measurementData = measurement;
  TOF_Imagers[TOF_ID].imager = imager;
  TOF_Imagers[TOF_ID].active = false;

  Wire.beginTransmission(0x44);
  if (Wire.endTransmission() == 0)
  {
    Wire.beginTransmission(0x44);
    if (Wire.endTransmission() == 0)
    {
      Serial.println("DEVICE0 CONNECTED AT 0x44: ");
      if (TOF_Imagers[0].imager.begin(0x44) == false)
      {
        Serial.println(F("Sensor0 not found - running scanner"));
      }
      else
      {
        TOF_Imagers[TOF_ID].active = true;
        SensorCount=1;
      }
    }
  }
  else
  {
    Serial.println("DEVICE0 CONNECTED AT 0x29: ");
    if (TOF_Imagers[0].imager.begin() == false)
    {
      Serial.println(F("Sensor0 not found - running scanner"));
    }

    if (TOF_Imagers[0].imager.setAddress(deviceAddress_0) == false)
    {
      Serial.println(F("Device0 failed to set new address. Please try again. Freezing..."));
      while (1);
    }
    else
    {
      Serial.println("DEVICE0 CHANGED TO 0x44: ");
      TOF_Imagers[TOF_ID].active = true;
      SensorCount=1;
    }
  }
  Serial.println("Enabling Sensor1");

  delay(500);
  digitalWrite(19, LOW);
  delay(500);

  TOF_ID = 1;
  TOF_Imagers[TOF_ID].TOF_ID = (TOF_Position_e) TOF_ID;
  SparkFun_VL53L5CX imager2;
  VL53L5CX_ResultsData measurement2;
  TOF_Imagers[TOF_ID].measurementData = measurement2;
  TOF_Imagers[TOF_ID].imager = imager2;
  TOF_Imagers[TOF_ID].active = false;

  Wire.beginTransmission(0x29);

  if (Wire.endTransmission() == 0)
  {
    
    if (TOF_Imagers[1].imager.begin(0x29) == false)
    {
      Serial.println(F("Sensor1 not found - running scanner"));
    }
    else
    {
      Serial.println("DEVICE1 CONNECTED AT 0x29: ");
      TOF_Imagers[TOF_ID].active = true;
      SensorCount=2;
    }
  }


  Serial.println("FINISHED INIT");

  delay(500);

  
  if (SensorCount>=1)
  {
    TOF_Imagers[0].active = true;
    TOF_Imagers[0].imager.setResolution(8 * 8); //Enable all 64 pads
    imageResolution = TOF_Imagers[0].imager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
    imageWidth = sqrt(imageResolution); //Calculate printing width
    TOF_Imagers[0].imager.startRanging();
  }

  if (SensorCount==2)
  {
    TOF_Imagers[1].active = true;
    TOF_Imagers[1].imager.setResolution(8 * 8); //Enable all 64 pads
    TOF_Imagers[1].imager.startRanging();
  }
}

void TOF_init(void) {

  pinMode(TOF_RST_PIN, OUTPUT); // Connected to RST Pin of one of the sensors
  digitalWrite(TOF_RST_PIN, HIGH);

  Wire.begin(); //This resets I2C bus to 100kHz
  Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz

  TOF_config_raw();
  // TOF_config(TOF_NE, 0x44, -1);
  // TOF_config(TOF_NW, 0x29, TOF_RST_PIN);

  Serial.printf("Successfully initialised %d TOFs.", TOF_NUM_TEST);
}

void TOF_scan(int TOF_ID) {
  if (TOF_Imagers[TOF_ID].imager.isDataReady() == true) {
    // Read distance data into array
    if (TOF_Imagers[TOF_ID].imager.getRangingData(&TOF_Imagers[TOF_ID].measurementData)) {
      //The ST library returns the data transposed from zone mapping shown in datasheet
      // Pretty-print data with increasing y, decreasing x to reflect reality
      Serial.printf("{ID: %d, grid: [", TOF_ID);
      for (int y = 0 ; y < TOF_RESOLUTION ; y += TOF_IMG_WIDTH) {
        for (int x = TOF_IMG_WIDTH - 1 ; x >= 0; x--) {
        Serial.print(TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y]);
        Serial.print(", ");
        }
      }
      Serial.printf("]}\n");
    }

  }
}

// void TOF_Grid_Processing(int TOF_ID, int* aggregate_columns) {
//   // Get data
//   VL53L5CX_ResultsData data = TOF_Imagers[TOF_ID].measurementData;
//   // get average per two columns
//   int column = 0;
//   for (int y = 0 ; y < TOF_RESOLUTION ; y += TOF_IMG_WIDTH) {
//     int min = TOF_Imagers[TOF_ID].measurementData.distance_mm[(TOF_IMG_WIDTH - 1) + y];
//     for (int x = TOF_IMG_WIDTH - 1 ; x >= 0; x--) {
//       if (TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y] < min) {
//         min = TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y];
//       }
//       aggregate_columns[x] = min;
//     }
//   }
// }

void TOF_Grid_Processing(int TOF_ID, int* aggregate_columns) {
  // Get data
  VL53L5CX_ResultsData data = TOF_Imagers[TOF_ID].measurementData;
  // get average per two columns
  int columns[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int c = 0;
  for (int y = 0 ; y < TOF_RESOLUTION ; y += TOF_IMG_WIDTH) {
    int min = TOF_Imagers[TOF_ID].measurementData.distance_mm[(TOF_IMG_WIDTH - 1) + y];
    for (int x = TOF_IMG_WIDTH - 3 ; x >= 0; x--) {
      if (TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y] < min) {
        min = TOF_Imagers[TOF_ID].measurementData.distance_mm[x + y];
      }
      columns[c] = min;
      
    }
    c++;
    Serial.printf("%d ", min);
  }
  
  Serial.printf("\n");

  for (int i = 0; i < TOF_IMG_WIDTH; i += 2) {
    if (columns[i] < columns[i+1]) {
      aggregate_columns[i/2] = columns[i];
    } else {
       aggregate_columns[i/2] = columns[i+1];
    }
  }

  Serial.printf("%d, %d, %d, %d\n", aggregate_columns[0], aggregate_columns[1], aggregate_columns[2], aggregate_columns[3]);
}


