/******************************************************************************
InterruptHWTapConfig.ino

Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/LSM6DS3_Breakout
https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library

Description:
Example using the LSM6DS3 interrupts.

This sketch demonstrates one way to detect single and double-tap events using
hardware interrupt pins. The LSM6DS3 pulses the int1 line once after the first
tap, then again if a second tap comes in.

The configuration is determined by reading the LSM6DS3 datasheet and application
note, then driving hex values to the registers of interest to set the appropriate
bits.  The sketch is based of the "LowLevelExampe" sketch.

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

Hardware connections:
Connect I2C SDA line to A4
Connect I2C SCL line to A5
Connect GND and 3.3v power to the IMU
Connect INT1 to pin 2 -- Note:  the atmega has 5v input and the LSM6DS3 is 3.3v
output.  This is OK because the input is high impedance and 3.3v is a logic '1'
for 5v processors.  The signal is correctly detected and nothing is damaged.

Do not configure pin 2 as OUTPUT!

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

LSM6DS3Core myIMU( I2C_MODE, 0x6B );
//LSM6DS3Core myIMU( SPI_MODE, 10 );

uint8_t TAP_SRC = 0;
uint8_t LED_level = 0b11;

void IRAM_ATTR int1ISR()
{
  //Serial.println("Interrupt serviced.");
  int1Status++;
}


void setup()
{
	Serial.begin(9600);
	delay(1000); //relax...
	Serial.println("Processor came out of reset.\n");

	//Call .beginCore() to configure the IMU
	if( myIMU.beginCore() != 0 )
	{
		Serial.print("Error at beginCore().\n");
	}
	else
	{
		Serial.print("\nbeginCore() passed.\n");
	}

	//Error accumulation variable
	uint8_t errorAccumulator = 0;

	uint8_t dataToWrite = 0;  //Temporary variable

	//Setup the accelerometer******************************
	dataToWrite = 0; //Start Fresh!
	dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
	dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
	dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

	// //Now, write the patched together data
	errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

	//Set the ODR bit
	errorAccumulator += myIMU.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
	dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

	// Enable tap detection on X, Y, Z axis, but do not latch output

	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_TAP_CFG1, 0x0E );
	
	// Set tap threshold
	// Write 0Ch into TAP_THS_6D
	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x0A );

	// Set Duration, Quiet and Shock time windows
	// Write 7Fh into INT_DUR2
	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_INT_DUR2, 0x3F );
	
	// Single & Double tap enabled (SINGLE_DOUBLE_TAP = 1)
	// Write 80h into WAKE_UP_THS
	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80 );
	
	// Single tap interrupt driven to INT1 pin -- enable latch
	// Write 08h into MD1_CFG
	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_MD1_CFG, 0x08 );

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
	attachInterrupt(33, int1ISR, RISING);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
}


void loop()
{
	if( int1Status > 0 )  //If ISR has been serviced at least once
	{
    myIMU.readRegister(&TAP_SRC, LSM6DS3_ACC_GYRO_TAP_SRC);
		//Check if there are more than one interrupt pulse
		if( TAP_SRC & 0b100000 )
		{
			Serial.print("Single-tap event\n");
		}
		if( TAP_SRC & 0b10000 )
		{
			Serial.print("Double-tap event\n");
		}
		Serial.println(TAP_SRC, BIN);
		//Clear the ISR counter
		int1Status = 0;
    TAP_SRC = 0;
    LED_level = LED_level ^ 0b1111;
    setLEDs(LED_level);
	}
	
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
