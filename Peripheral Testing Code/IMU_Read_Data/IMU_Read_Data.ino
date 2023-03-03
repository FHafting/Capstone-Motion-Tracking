#include <LSM6DSOSensor.h>
#include <lsm6dso_reg.h>

#define I2C_SDA 21
#define I2C_SCL 22

#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18
#define CS_PIN 5

TwoWire dev_i2c = TwoWire(0);
LSM6DSOSensor AccGyr(&dev_i2c);

//SPIClass *dev_spi = new SPIClass(VSPI);
//LSM6DSOSensor AccGyr(dev_spi, CS_PIN);

void setup() {
  // put your setup code here, to run once:
  dev_i2c.begin();
  AccGyr.begin();  
  AccGyr.Enable_X();  
  AccGyr.Enable_G();
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int32_t accelerometer[3];
  int32_t gyroscope[3];
  AccGyr.Get_X_Axes(accelerometer);  
  AccGyr.Get_G_Axes(gyroscope);

//  Serial.print(accelerometer[0]);
//  Serial.print(", ");
//  Serial.print(accelerometer[1]);
//  Serial.print(", ");
//  Serial.println(accelerometer[3]);
//  Serial.print(", ");
//  
  Serial.print(gyroscope[0]);
  Serial.print(", ");
  Serial.print(gyroscope[1]);
  Serial.print(", ");
  Serial.println(gyroscope[2]);
}
