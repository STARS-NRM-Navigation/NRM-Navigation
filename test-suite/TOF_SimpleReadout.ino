/*
  Read an 8x8 array of distances from the VL53L5CX
  By: Nathan Seidle
  SparkFun Electronics
  Date: October 26, 2021
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to set a custom address for the VL53L5CX.

  Note: The I2C address for the device is stored in NVM so it will have to be set
  at each power on.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/18642
*/

#include <Wire.h>

#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

SparkFun_VL53L5CX myImager_0;
SparkFun_VL53L5CX myImager_1;

int deviceAddress_0 = 0x44; 
int deviceAddress_1 = 0x29; 

int SensorCount = 0;

VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM
VL53L5CX_ResultsData measurementData1; // Result data class structure, 1356 byes of RAM

int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output

int sensorReset1 = 19; //GPIO that is connected to the Reset pin on sensor 1


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("SparkFun VL53L5CX Imager Example");

  pinMode(sensorReset1, OUTPUT); // Connected to RST Pin of one of the sensors
  digitalWrite(sensorReset1, HIGH); 

  Wire.begin(); //This resets I2C bus to 100kHz
  Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz

  Serial.println(F("Initializing sensor board. This can take up to 10s. Please wait."));




  Wire.beginTransmission(0x44);
  if (Wire.endTransmission() == 0)
  {
    Wire.beginTransmission(0x44);
    if (Wire.endTransmission() == 0)
    {
      Serial.println("DEVICE0 CONNECTED AT 0x44: ");
      if (myImager_0.begin(0x44) == false)
      {
        Serial.println(F("Sensor0 not found - running scanner"));
        i2cScanner();
      }
      else
      {
        SensorCount=1;
      }
    }
  }
  else
  {
    Serial.println("DEVICE0 CONNECTED AT 0x29: ");
    if (myImager_0.begin() == false)
    {
      Serial.println(F("Sensor0 not found - running scanner"));
      i2cScanner();
    }

    if (myImager_0.setAddress(deviceAddress_0) == false)
    {
      Serial.println(F("Device0 failed to set new address. Please try again. Freezing..."));
      while (1);
    }
    else
    {
      Serial.println("DEVICE0 CHANGED TO 0x44: ");
      SensorCount=1;
    }
  }
  Serial.println("Enabling Sensor1");
  delay(500);
  digitalWrite(sensorReset1, LOW);
  delay(500);

  Wire.beginTransmission(0x29);
  if (Wire.endTransmission() == 0)
  {
    
    if (myImager_1.begin(0x29) == false)
    {
      Serial.println(F("Sensor1 not found - running scanner"));
      i2cScanner();
    }
    else
    {
      Serial.println("DEVICE1 CONNECTED AT 0x29: ");
      SensorCount=2;
    }
  }


  Serial.println("FINISHED INIT");

  delay(500);

  
  if (SensorCount>=1)
  {
    myImager_0.setResolution(8 * 8); //Enable all 64 pads
    imageResolution = myImager_0.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
    imageWidth = sqrt(imageResolution); //Calculate printing width
    myImager_0.startRanging();
  }

  if (SensorCount==2)
  {
    myImager_1.setResolution(8 * 8); //Enable all 64 pads
    myImager_1.startRanging();
  }

 
}

void loop()
{
  //Poll sensor for new data
  if (SensorCount>=1)
  {
    Serial.println("Sensor 0");
    if (myImager_0.isDataReady() == true)
    {
      if (myImager_0.getRangingData(&measurementData)) //Read distance data into array
      {
        //The ST library returns the data transposed from zone mapping shown in datasheet
        //Pretty-print data with increasing y, decreasing x to reflect reality
        for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
        {
          for (int x = imageWidth - 1 ; x >= 0 ; x--)
          {
            Serial.print("\t");
            Serial.print(measurementData.distance_mm[x + y]);
          }
          Serial.println();
        }
        Serial.println();
      }
    }
  }

  if (SensorCount==2)
  {
    Serial.println("Sensor 1");
    if (myImager_1.isDataReady() == true)
    {
      if (myImager_1.getRangingData(&measurementData1)) //Read distance data into array
      {
        //The ST library returns the data transposed from zone mapping shown in datasheet
        //Pretty-print data with increasing y, decreasing x to reflect reality
        for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
        {
          for (int x = imageWidth - 1 ; x >= 0 ; x--)
          {
            Serial.print("\t");
            Serial.print(measurementData1.distance_mm[x + y]);
          }
          Serial.println();
        }
        Serial.println();
      }
    }
  }




  delay(1000); //Small delay between polling
}

void i2cScanner()
{
  Serial.println(F("Scanning for I2C devices"));
  for (byte address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Device found at address 0x");
      if (address < 0x10)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  Serial.println("Done");
  delay(250);

}
