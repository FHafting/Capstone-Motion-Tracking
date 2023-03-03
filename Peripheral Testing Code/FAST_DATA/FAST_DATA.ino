/******************************************************************************
MinimalistExample.ino

Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/LSM6DS3_Breakout
https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library

Description:
Most basic example of use.

Example using the LSM6DS3 with basic settings.  This sketch collects Gyro and
Accelerometer data every second, then presents it on the serial monitor.

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation
Either can be omitted if not used

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

Hardware connections:
Connect I2C SDA line to A4
Connect I2C SCL line to A5
Connect GND and 3.3v power to the IMU

This code is released under the [MIT License](http://opensource.org/licenses/MIT).

Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.

Distributed as-is; no warranty is given.
******************************************************************************/

#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"

LSM6DS3 myIMU; //Default constructor is I2C, addr 0x6B

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  delay(1000); //relax...
  Serial.println("Processor came out of reset.\n");

  myIMU.settings.gyroEnabled = 1;  //Can be 0 or 1
  myIMU.settings.gyroRange = 2000;   //Max deg/s.  Can be: 125, 245, 500, 1000, 2000
  myIMU.settings.gyroSampleRate = 1666;   //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666
  myIMU.settings.gyroBandWidth = 200;  //Hz.  Can be: 50, 100, 200, 400;
  myIMU.settings.gyroFifoEnabled = 0;  //Set to include gyro in FIFO
  myIMU.settings.gyroFifoDecimation = 1;  //set 1 for on /1

  myIMU.settings.accelEnabled = 1;
  myIMU.settings.accelRange = 16;      //Max G force readable.  Can be: 2, 4, 8, 16
  myIMU.settings.accelSampleRate = 1666;  //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
  myIMU.settings.accelBandWidth = 200;  //Hz.  Can be: 50, 100, 200, 400;
  myIMU.settings.accelFifoEnabled = 0;  //Set to include accelerometer in the FIFO
  myIMU.settings.accelFifoDecimation = 1;  //set 1 for on /1
  myIMU.settings.tempEnabled = 1;
  
    //Non-basic mode settings
  myIMU.settings.commMode = 1;

  //FIFO control settings
  myIMU.settings.fifoThreshold = 100;  //Can be 0 to 4096 (16 bit bytes)
  myIMU.settings.fifoSampleRate = 50;  //Hz.  Can be: 10, 25, 50, 100, 200, 400, 800, 1600, 3300, 6600
  myIMU.settings.fifoModeWord = 0;  //FIFO mode.
  //FIFO mode.  Can be:
  //  0 (Bypass mode, FIFO off)
  //  1 (Stop when full)
  //  3 (Continuous during trigger)
  //  4 (Bypass until trigger)
  //  6 (Continous mode)
  
  //Call .begin() to configure the IMU
  myIMU.begin();
  //default settings
  // Accel: 400Hz BW, 

  pinMode(12, OUTPUT);
  
}


void loop()
{
  //Get all parameters
//  Serial.print("\nAccelerometer:\n");
//  Serial.print(micros());
//  Serial.print(", ");
  digitalWrite(12, HIGH);
  Serial.print(myIMU.readFloatAccelX(), 4);
  Serial.print(", ");
  Serial.print(myIMU.readFloatAccelY(), 4);
  Serial.print(", ");
  Serial.print(myIMU.readFloatAccelZ(), 4);
//  digitalWrite(12, LOW);
//  delay(1);

//  digitalWrite(12, HIGH);
//  Serial.print("\nGyroscope:\n");
  Serial.print(", ");
  Serial.print(myIMU.readFloatGyroX(), 4);
  Serial.print(", ");
  Serial.print(myIMU.readFloatGyroY(), 4);
  Serial.print(", ");
  Serial.println(myIMU.readFloatGyroZ(), 4);
  digitalWrite(12, LOW);
  delay(1);

//  digitalWrite(12,HIGH);
//  myIMU.readFloatAccelX();
//  myIMU.readFloatAccelY();
//  myIMU.readFloatAccelZ();
//  myIMU.readFloatGyroX();
//  myIMU.readFloatGyroY();
//  myIMU.readFloatGyroZ();
//  digitalWrite(12,LOW);
//  delayMicroseconds(100);

//  Serial.print("\nThermometer:\n");
//  Serial.print(" Degrees C = ");
//  Serial.println(myIMU.readTempC(), 4);
//  Serial.print(" Degrees F = ");
//  Serial.println(myIMU.readTempF(), 4);
  
//  delay(5);
}
