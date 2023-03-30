#include "headers.h"




// receiver's mac address
//uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x3D, 0xD3, 0xD8};
//uint8_t broadcastAddress[] = {0xf0, 0x08, 0xd1, 0xd4, 0x56, 0x14};
//uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x30, 0xa0, 0x90};
uint8_t broadcastAddress[] = {0xa4, 0xcf, 0x12, 0x04, 0xe3, 0xe4};
int mcuID = 2;    //id goes from 0 to 5

// initializing time

const int packetSize = 10; // amount of sensor readings being sent within each packet
// transfer rate = packetSize*10 (ms) - 100ms (current)
// transfer rate = 1/(packetSize*10/1000) (Hz) - 10Hz (current)
// keep in mind this still sends data equivalent to 100Hz


// defining the sample rate for every sensor reading
unsigned long long cm = 0;
unsigned long long pm = 0;
int interval = 10; // 10 ms interval for 100 hz
int counter = 0;   // variable to sequentially store packets as fixed packet sizes


//initializing status led
int ledPin1 = 2;
int ledStatus1 = false;

//function to toggle LED pin 2
void toggleLED(){
  if(ledStatus1==true){
    ledStatus1=false;
  }
  else{
    ledStatus1=true;
  }
  digitalWrite(ledPin1,ledStatus1);
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
  Serial.print("\r\nLast Packet Send Status:\t");

  if(status== ESP_NOW_SEND_SUCCESS){
    Serial.println("Delivery Success");
    toggleLED();

  }else{
    //turning off LED
    Serial.println("Delivery Fail");
      ledStatus1=false;
      digitalWrite(ledPin1,ledStatus1);
  }


 // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//********************************************************************************************
//LSM6DS3 myIMU; //Default constructor is I2C, addr 0x6B
//********************************************************************************************


void setup()
{
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
  //initialise status led
  pinMode(ledPin1,OUTPUT);

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


void loop()
{

 
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
    // myData.accelx[counter] = myIMU.readFloatAccelX();
    // myData.accely[counter] = myIMU.readFloatAccelY();
    // myData.accelz[counter] = myIMU.readFloatAccelZ();
    // myData.gyrox[counter] = myIMU.readFloatGyroX();
    // myData.gyroy[counter] = myIMU.readFloatGyroY();
    // myData.gyroz[counter] = myIMU.readFloatGyroZ();

   myData.accelx[counter] = accelxVal();
    myData.accely[counter] = accelyVal();
    myData.accelz[counter] = accelzVal();
    myData.gyrox[counter] = gyroxVal();
    myData.gyroy[counter] = gyroyVal();
    myData.gyroz[counter] = gyrozVal();
//********************************************************************************************
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
      Serial.println("Sent with success");
      
    }
    else
    {
      Serial.println("Error sending the data");
      
    }
  }
}


