/**
 ******************************************************************************
 * @file tof.cpp
 * @author Rachel Chiong
 * @brief TOF header file
 ******************************************************************************
 */

#ifndef __NRM_TOF_H__
#define __NRM_TOF_H__

/* === Includes ============================================================ */

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>

/* === Defines ============================================================= */

#define TOF_NUM 8
#define TOF_RESOLUTION 64 // 8 x 8 array
#define TOF_IMG_WIDTH 8

#define TOF_NUM_TEST 2

#define I2C_MUX_ADDR 0x70

/* === Typedefs ============================================================ */

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

/* === Global Variables ==================================================== */

// Store Imagers in an array
extern TOF_Imager_t TOF_Imagers[8];

extern SparkFun_VL53L5CX testImager;
extern VL53L5CX_ResultsData measurementData;

extern int imageResolution; //Used to pretty print output
extern int imageWidth; //Used to pretty print output

/* === Public Functions ==================================================== */

void TOF_MUX_Select(int TOF_ID);
void TOF_init(void);
void TOF_scan(int TOF_ID);
void TOF_Grid_Processing(int TOF_ID, int* new_aggregate_columns);
void TOF_Grid_Processing_min(int TOF_ID, int* new_aggregate_columns);

#endif // __NRM_LED_H__
