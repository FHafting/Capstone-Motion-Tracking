#include "headers.h"




LSM6DSO myIMU; //Default constructor is I2C, addr 0x6B

// receiver's mac address
// uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x3D, 0xD3, 0xD8};
// uint8_t broadcastAddress[] = {0xf0, 0x08, 0xd1, 0xd4, 0x56, 0x14};
// uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x30, 0xa0, 0x90};
uint8_t broadcastAddress[] = {0xa4, 0xcf, 0x12, 0x04, 0xe3, 0xe4};
int mcuID = 1; // id goes from 0 to 5

// initializing time

const int packetSize = 10; // amount of sensor readings being sent within each packet
// transfer rate = packetSize*10 (ms) - 100ms (current)
// transfer rate = 1/(packetSize*10/1000) (Hz) - 10Hz (current)
// keep in mind this still sends data equivalent to 100Hz

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

typedef struct struct_signal
{
  int time;
} struct_signal;

struct_signal resetSig;

float accelx=0;
  float accely=0;
  float accelz=0;
  float gyrox=0;
  float gyroy=0;
  float gyroz=0;

// defining the sample rate for every sensor reading
unsigned long long cm = 0;
unsigned long long pm = 0;
int interval = 10; // 10 ms interval for 100 hz
int counter = 0;   // variable to sequentially store packets as fixed packet sizes
unsigned long long prevTime=0;

unsigned long long cm2 = 0;
unsigned long long pm2 = 0;
int interval2 = 10; // 10 ms interval for 100 hz
int counter2 = 0;
int reset1 = 0; // 0 = waiting for signal, 1 = signal received

// initializing status led
int ledPin1 = 2;
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

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");

  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("Delivery Success");
    toggleLED();
  }
  else
  {
    // turning off LED
    Serial.println("Delivery Fail");
    ledStatus1 = false;
    digitalWrite(ledPin1, ledStatus1);
  }

  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&resetSig, incomingData, sizeof(resetSig));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("time ");
  Serial.println(resetSig.time);
 
    prevTime = millis();
    pm=0;
  
}



void setup()
{
 
 
  Wire.begin(23,22);
  myIMU.begin();
  myIMU.initialize(BASIC_SETTINGS);
   

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

  esp_now_register_recv_cb(OnDataRecv);
}

//********************************************************************************************
float accelxVal(float data)
{
  return data;
}
float accelyVal(float data)
{
  return data;
}
float accelzVal(float data)
{
  return data;
}
float gyroxVal(float data)
{
  return data;
}
float gyroyVal(float data)
{
  return data;
}
float gyrozVal(float data)
{
  return data;
}
//********************************************************************************************



void sendingSensorData()
{
  if (counter == 0)
  {
    myData.time = pm;
  }

  if ((cm - pm) >= interval)
  {
    
    pm = cm;
    // update data
    myData.id = mcuID;

    //********************************************************************************************
    myData.accelx[counter] = accelx;
    myData.accely[counter] = accely;
    myData.accelz[counter] = accelz;
    myData.gyrox[counter] = gyrox;
    myData.gyroy[counter] = gyroy;
    myData.gyroz[counter] = gyroz;
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
     // Serial.println("Sent with success");
    }
    else
    {
     // Serial.println("Error sending the data");
    }
  }
}

void loop()
{


  accelx = myIMU.readFloatAccelX();
  accely = myIMU.readFloatAccelY();
  accelz = myIMU.readFloatAccelZ();
  gyrox = myIMU.readFloatGyroX();
  gyroy = myIMU.readFloatGyroY();
  gyroz = myIMU.readFloatGyroZ();


  cm = millis()- prevTime ;
  cm2 = millis();
  sendingSensorData();

}

