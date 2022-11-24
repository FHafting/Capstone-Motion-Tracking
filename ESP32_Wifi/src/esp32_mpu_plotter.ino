#include "headers.h"
//#include "functions.h"

// Replace with your network credentials
const char *ssidSTA = "CANADIAN WINTER BELL";
const char *passwordSTA = "Shivam1912__";

const char *ssidSTA2 = "iPhone (100)";
const char *passwordSTA2 = "shiv1912";

const char *ssidAP = "ESP32_Shiv";
const char *passwordAP = "12345678";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const int MPU_ADDR = 0x68;            // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int gyro_x, gyro_y, gyro_z;           // variables for gyro raw data
int temperature;                      // variables for temperature data

void mpuSetup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B);                 // PWR_MGMT_1 register
  Wire.write(0);                    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

String mpuRead(int index)
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                        // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false);             // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

  float result[7];
  result[0] = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  result[1] = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  result[2] = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  result[3] = Wire.read() << 8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  result[4] = Wire.read() << 8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  result[5] = Wire.read() << 8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  result[6] = Wire.read() << 8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  float result2;
  switch (index)
  {
  case 0:
    result2 = result[0];
    break;
  case 1:
    result2 = result[1];
    break;
  case 2:
    result2 = result[2];
    break;
  case 3:
    result2 = result[3];
    break;
  case 4:
    result2 = result[4];
    break;
  case 5:
    result2 = result[5];
    break;
  case 6:
    result2 = result[6];
    break;
  default:
    result2 = 0;
    break;
  }

  if (isnan(result2))
  {
    Serial.println("Failed to read MPU6050 Value!");
    return "";
  }
  else
  {
    Serial.println(result2);
    return String(result2);
  }
}

void spiffsSetup()
{
  // Initialize SPIFFS
  SPIFFS.begin(true);
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void wifiSetup()
{
  // Connect to Wi-Fi
  // WiFi.softAP(ssidAP, passwordAP);
  WiFi.begin(ssidSTA, passwordSTA);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  // Serial.println(WiFi.softAPIP());
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  mpuSetup();
  spiffsSetup();
  wifiSetup();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html"); });
  server.on("/accelx", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", mpuRead(0).c_str()); });
            server.on("/accely", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", mpuRead(1).c_str()); });
            server.on("/accelz", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", mpuRead(2).c_str()); });

  // Start server
  server.begin();
}

void loop()
{

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
}
