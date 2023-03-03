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

//Interrupt variables
#define int1Pin 33  //Use pin 2 for int.0 on uno
uint8_t int1Status = 0;

#define LED1 12
#define LED2 27
#define LED3 26
#define LED4 25

LSM6DS3 myIMU; //Default constructor is I2C, addr 0x6B

uint8_t TAP_SRC = 0;
uint8_t LED_level = 0b11;

bool set_tap_time = true;
long last_tap_time = 0;
int tap_wait = 400;

void IRAM_ATTR int1ISR()
{
  //Serial.println("Interrupt serviced.");
  int1Status++;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  delay(1000); //relax...
  Serial.println("Processor came out of reset.\n");

  myIMU.settings.gyroEnabled = 1;  //Can be 0 or 1
  myIMU.settings.gyroRange = 2000;   //Max deg/s.  Can be: 125, 245, 500, 1000, 2000
  myIMU.settings.gyroSampleRate = 833;   //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666
  myIMU.settings.gyroBandWidth = 200;  //Hz.  Can be: 50, 100, 200, 400;
  myIMU.settings.gyroFifoEnabled = 0;  //Set to include gyro in FIFO
  myIMU.settings.gyroFifoDecimation = 1;  //set 1 for on /1

  myIMU.settings.accelEnabled = 1;
  myIMU.settings.accelRange = 16;      //Max G force readable.  Can be: 2, 4, 8, 16
  myIMU.settings.accelSampleRate = 833;  //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
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

    //Error accumulation variable
  uint8_t errorAccumulator = 0;

    // Enable tap detection on X, Y, Z axis, but do not latch output

  errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_TAP_CFG1, 0x0E );
  
  // Set tap threshold
  // Write 0Ch into TAP_THS_6D
  errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x03 );

  // Set Duration, Quiet and Shock time windows
  // Write 7Fh into INT_DUR2
  errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_INT_DUR2, 0x7F );
  
  // Single & Double tap enabled (SINGLE_DOUBLE_TAP = 1)
  // Write 80h into WAKE_UP_THS
  errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80 );
  
  // Single tap interrupt driven to INT1 pin -- enable latch
  // Write 08h into MD1_CFG
  errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_MD1_CFG, 0x40 );

  if( errorAccumulator )
  {
    Serial.println("Problem configuring the device.");
  }
  else
  {
    Serial.println("Device O.K.");
  } 

  //Configure the atmega interrupt pin
  pinMode(int1Pin, INPUT);
  attachInterrupt(int1Pin, int1ISR, RISING);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  
}


void loop()
{
  //Get all parameters
//  Serial.print("\nAccelerometer:\n");
//  Serial.print(micros());
//  Serial.print(", ");
//  digitalWrite(12, HIGH);
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
//  digitalWrite(12, LOW);
//  delay(1);

//  myIMU.readFloatAccelX();
//  myIMU.readFloatAccelY();
//  myIMU.readFloatAccelZ();
//  myIMU.readFloatGyroX();
//  myIMU.readFloatGyroY();
//  myIMU.readFloatGyroZ();

  //digitalWrite(LED4,HIGH);
  if( int1Status > 0 )  //If ISR has been serviced at least once
  {
    if(set_tap_time){
      last_tap_time = millis();
      set_tap_time = false;
    }
    //Wait for a window (in case a second tap is coming)
    if(millis() - last_tap_time > tap_wait){
      //Check if there are more than one interrupt pulse
      if( int1Status == 1 )
      {
        Serial.print("Single-tap event\n");
        
      }
      if( int1Status > 1 )
      {
        Serial.print("Double-tap event\n");
        setLEDs(LED_level);
        LED_level = LED_level ^ 0b1111;
      }
      
      //Clear the ISR counter
      int1Status = 0;
      set_tap_time = true;
    }
  }
  //digitalWrite(LED4,LOW);
  //delayMicroseconds(100);

//  Serial.print("\nThermometer:\n");
//  Serial.print(" Degrees C = ");
//  Serial.println(myIMU.readTempC(), 4);
//  Serial.print(" Degrees F = ");
//  Serial.println(myIMU.readTempF(), 4);
  
//  delay(5);
}

void setLEDs(uint8_t LED_level){
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  
  if(LED_level & 0b1)
    digitalWrite(LED1, HIGH);
  if(LED_level & 0b10)
    digitalWrite(LED2, HIGH);
  if(LED_level & 0b100)
    digitalWrite(LED3, HIGH);
  if(LED_level & 0b1000)
    digitalWrite(LED4, HIGH);
}
