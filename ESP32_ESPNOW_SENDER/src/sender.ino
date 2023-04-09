#include "headers.h"

// receiver's mac address
// uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x3D, 0xD3, 0xD8};
// uint8_t broadcastAddress[] = {0xf0, 0x08, 0xd1, 0xd4, 0x56, 0x14};
//40:22:D8:6A:C4:34

 uint8_t broadcastAddress[] = {0x40, 0x22, 0xD8, 0x6A, 0xC4, 0x34};

//uint8_t broadcastAddress[] = {0xa4, 0xcf, 0x12, 0x04, 0xe3, 0xe4};
int mcuID = 0 ; // id goes from 0 to 9

// initializing time
// LSM6DSO myIMU;             // Default constructor is I2C, addr 0x6B
const int packetSize = 10; // amount of sensor readings being sent within each packet
// transfer rate = packetSize*10 (ms) - 100ms (current)
// transfer rate = 1/(packetSize*10/1000) (Hz) - 10Hz (current)
// keep in mind this still sends data equivalent to 100Hz

//***********************************************************************************************************************

#define LED1 13
#define LED2 14
#define LED3 15
// #define LED4 12

uint8_t LED_level = 0b0101;

#define motorPin 25
int pulses = 4;
int pulseNum = 1;
bool motorState = 1;
bool pulseMotor = false;
long lastPulseTime = 0;
int pulseDuration = 400;

// after vibration motor, wait for device to stabilize
bool tapEnable = true;
bool settle = false;
int pulseSettleDuration = 200;
long pulseSettleStart = 0;

LSM6DSO myIMU;

// Interrupt variables
#define int1Pin 21 // Use pin 2 for int.0 on uno
uint8_t int1Status = 0;
uint8_t TAP_SRC_REGISTER = 0;

void IRAM_ATTR int1ISR()
{
  // Serial.println("Interrupt serviced.");
  int1Status++;
}

void setLEDs(uint8_t LED_level){
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  //digitalWrite(LED4, LOW);
  
  if(LED_level & 0b1)
    digitalWrite(LED1, HIGH);
  if(LED_level & 0b10)
    digitalWrite(LED2, HIGH);
  if(LED_level & 0b100)
    digitalWrite(LED3, HIGH);
  //if(LED_level & 0b1000)
   // digitalWrite(LED4, HIGH);
}

//***********************************************************************************************************************

// defining the sample rate for every sensor reading
unsigned long long cm = 0;
unsigned long long pm = 0;
int interval = 10; // 10 ms interval for 100 hz
int counter = 0;   // variable to sequentially store packets as fixed packet sizes

// initializing status led
int ledPin1 = 12;
int ledStatus1 = false;

// function to toggle LED pin 2
void toggleLED()
{
  if (ledStatus1 == true)
  {
    ledStatus1 = false;
  }
  else
  {
    ledStatus1 = true;
  }
  digitalWrite(ledPin1, ledStatus1);
}

// Must match the sender structure
typedef struct struct_message
{
  int id;
  unsigned long int time;
  float accelx[packetSize];
  float accely[packetSize];
  float accelz[packetSize];
  float gyrox[packetSize];
  float gyroy[packetSize];
  float gyroz[packetSize];
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  // Serial.print("\r\nLast Packet Send Status:\t");

  if (status == ESP_NOW_SEND_SUCCESS)
  {
    // Serial.println("Delivery Success");
    toggleLED();
  }
  else
  {
    // turning off LED
    // Serial.println("Delivery Fail");
    ledStatus1 = false;
    digitalWrite(ledPin1, ledStatus1);
  }

  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//********************************************************************************************
// LSM6DS3 myIMU; //Default constructor is I2C, addr 0x6B
//********************************************************************************************

void setup()
{

  

  Wire.begin(23, 22);
  myIMU.begin();
  myIMU.initialize(SOFT_INT_SETTINGS);

  //*******************************************************************************************************************
  
  uint8_t errorAccumulator = 0;
  
  errorAccumulator += myIMU.writeRegister(CTRL1_XL, 0x60);
  errorAccumulator += myIMU.writeRegister(TAP_CFG0, 0x0E);
  errorAccumulator += myIMU.writeRegister(TAP_CFG1, 0x0C);
  errorAccumulator += myIMU.writeRegister(TAP_CFG2, 0x8C);
  errorAccumulator += myIMU.writeRegister(TAP_THS_6D, 0x0C);
  errorAccumulator += myIMU.writeRegister(INT_DUR2, 0x7F);
  errorAccumulator += myIMU.writeRegister(WAKE_UP_THS, 0x80);
  errorAccumulator += myIMU.writeRegister(MD2_CFG, 0x08);

  //Configure the atmega interrupt pin
  pinMode(int1Pin, INPUT);
  attachInterrupt(int1Pin, int1ISR, RISING);

  pinMode(motorPin, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  //pinMode(LED4, OUTPUT);
  setLEDs(LED_level);

  //*******************************************************************************************************************
  //********************************************************************************************
  // myIMU.settings.gyroEnabled = 1;  //Can be 0 or 1
  // myIMU.settings.gyroRange = 2000;   //Max deg/s.  Can be: 125, 245, 500, 1000, 2000
  // myIMU.settings.gyroSampleRate = 1666;   //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666
  // myIMU.settings.gyroBandWidth = 200;  //Hz.  Can be: 50, 100, 200, 400;
  // myIMU.settings.gyroFifoEnabled = 0;  //Set to include gyro in FIFO
  // myIMU.settings.gyroFifoDecimation = 1;  //set 1 for on /1

  // myIMU.settings.accelEnabled = 1;
  // myIMU.settings.accelRange = 16;      //Max G force readable.  Can be: 2, 4, 8, 16
  // myIMU.settings.accelSampleRate = 1666;  //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
  // myIMU.settings.accelBandWidth = 200;  //Hz.  Can be: 50, 100, 200, 400;
  // myIMU.settings.accelFifoEnabled = 0;  //Set to include accelerometer in the FIFO
  // myIMU.settings.accelFifoDecimation = 1;  //set 1 for on /1
  // myIMU.settings.tempEnabled = 1;

  //   //Non-basic mode settings
  // myIMU.settings.commMode = 1;

  // //FIFO control settings
  // myIMU.settings.fifoThreshold = 100;  //Can be 0 to 4096 (16 bit bytes)
  // myIMU.settings.fifoSampleRate = 50;  //Hz.  Can be: 10, 25, 50, 100, 200, 400, 800, 1600, 3300, 6600
  // myIMU.settings.fifoModeWord = 0;  //FIFO mode.
  // //FIFO mode.  Can be:
  // //  0 (Bypass mode, FIFO off)
  // //  1 (Stop when full)
  // //  3 (Continuous during trigger)
  // //  4 (Bypass until trigger)
  // //  6 (Continous mode)

  // //Call .begin() to configure the IMU
  // myIMU.begin();

  //********************************************************************************************

  // Init Serial Monitor
  Serial.begin(115200);
  // initialise status led
  pinMode(ledPin1, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

//********************************************************************************************
float accelxVal()
{
  return (float)(rand());
}
float accelyVal()
{
  return (float)(rand());
}
float accelzVal()
{
  return (float)(rand());
}
float gyroxVal()
{
  return (float)(rand());
}
float gyroyVal()
{
  return (float)(rand());
}
float gyrozVal()
{
  return (float)(rand());
}
//********************************************************************************************

int data = 0;
int i = 0;

void loop()
{
  data = myIMU.listenDataReady();

  if (counter == 0)
  {
    myData.time = pm;
  }

  cm = millis();
  if ((cm - pm) >= interval)
  {
    pm = cm;
    // update data
    myData.id = mcuID;

    //********************************************************************************************
    myData.accelx[counter] = myIMU.readFloatAccelX();
    myData.accely[counter] = myIMU.readFloatAccelY();
    myData.accelz[counter] = myIMU.readFloatAccelZ();
    myData.gyrox[counter] = myIMU.readFloatGyroX();
    myData.gyroy[counter] = myIMU.readFloatGyroY();
    myData.gyroz[counter] = sin(((float)i) * PI / 180);

    //  myData.accelx[counter] = accelxVal();
    //   myData.accely[counter] = accelyVal();
    //   myData.accelz[counter] = accelzVal();
    //   myData.gyrox[counter] = gyroxVal();
    //   myData.gyroy[counter] = gyroyVal();
    //   myData.gyroz[counter] = gyrozVal();
    //********************************************************************************************

    Serial.println(myData.gyroz[counter]);

    i++;
    if (i >= 360)
    {
      i = 0;
    }
    counter++; // incrementing counter within the loop signifying that an interval of 10 ms has passed
  }

  // send data via esp now
  if (counter >= packetSize)
  {
    counter = 0;
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

    if (result == ESP_OK)
    {
      // Serial.println("Sent with success");
    }
    else
    {
      // Serial.println("Error sending the data");
    }
  }



  // put your main code here, to run repeatedly:
  if(pulseMotor){
    if(millis() - lastPulseTime >= pulseDuration){
      lastPulseTime = millis();
      motorState ^= 0b1;
      digitalWrite(motorPin, motorState);
      LED_level ^= 0b1111;
      setLEDs(LED_level);
      pulseNum++;
    }
    if(pulseNum >= pulses){
      pulseMotor = false;
      settle = true;
      pulseSettleStart = millis();
      pulseNum = 1;
    }
  }
  if(settle){
    if(millis() - pulseSettleStart >= pulseSettleDuration){
      settle = false;
      tapEnable = true;
    }
  }
  if( int1Status > 0 && tapEnable)  //If ISR has been serviced at least once
  {
    myIMU.readRegister(&TAP_SRC_REGISTER, TAP_SRC);
    if(TAP_SRC_REGISTER && 0x10){   //only double taps
      Serial.print("Double-tap event\n");
      LED_level ^= 0b1111;
      setLEDs(LED_level);
      //Clear the ISR counter
      int1Status = 0;
      tapEnable = false;
      pulseMotor = true;
      motorState = 1;
      digitalWrite(motorPin, motorState);
      lastPulseTime = millis();
    }
  }
}