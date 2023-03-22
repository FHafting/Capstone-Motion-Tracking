#include "headers.h"
// #include "functions.h"

// receiver's mac address A4:CF:12:04:E3:E4
uint8_t broadcastAddress[] = {0xA4, 0xCF, 0x12, 0x04, 0xE3, 0xE4};
int mcuID = 1;    //id goes from 0 to 5

// initializing time

const int packetSize = 9; // amount of sensor readings being sent within each packet
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
  boolean espStatus;
  unsigned long long int time;
  int accelx[packetSize];
  int accely[packetSize];
  int accelz[packetSize];
  int gyrox[packetSize];
  int gyroy[packetSize];
  int gyroz[packetSize];
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

void setup()
{
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

// data functions
// samples sensor values every 10 ms (100 hz refresh rate)

//replace content of these functions with code for extracting sensor data from IMU

int accelxVal()
{
  int value = 12345;

  return value;
}
int accelyVal()
{
  int value = 12121;

  return value;
}
int accelzVal()
{
  int value = 00000;

  return value;
}
int gyroxVal()
{
  int value = 11111;

  return value;
}
int gyroyVal()
{
  int value = 99999;

  return value;
}
int gyrozVal()
{
  int value = 88888;

  return value;
}



void loop()
{

  // update time and status
  myData.espStatus=true;
  if (counter == 0)
  {
    myData.time = 14 * 60 * 60 * 1000 + 9 * 60 * 1000 + pm;
  }

  cm = millis();
  if ((cm - pm) >= interval)
  {
    pm = cm;
    // update data
    myData.id = mcuID;
    myData.accelx[counter] = accelxVal();
    myData.accely[counter] = accelyVal();
    myData.accelz[counter] = accelzVal();
    myData.gyrox[counter] = gyroxVal();
    myData.gyroy[counter] = gyroyVal();
    myData.gyroz[counter] = gyrozVal();
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


