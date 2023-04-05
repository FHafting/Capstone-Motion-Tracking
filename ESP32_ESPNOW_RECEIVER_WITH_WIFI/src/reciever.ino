#include "headers.h"
// #include "functions.h"

const int devices = 10;    // must add boards below and in boardsStruct
const int packetSize = 10; // amount of sensor readings being sent within each packet

uint8_t broadcastAddresses[10][6] = {
    {0x3c, 0x61, 0x05, 0x30, 0xA0, 0x90},
    {0xf0, 0x08, 0xd1, 0xd4, 0x56, 0x14},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

typedef struct struct_signal
{
  int time;
} struct_signal;

struct_signal resetSig;

const char *ssidAP = "ESP32_WiFi";
const char *passwordAP = "12345678";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// initializing for sending time
esp_now_peer_info_t peerInfo;
// initializing message structure from ESP NOW
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

// Creating upto 6 esp 32 devices
struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;
struct_message board5;
struct_message board6;
struct_message board7;
struct_message board8;
struct_message board9;
struct_message board10;

// Create an array with all the structures
struct_message boardsStruct[devices] = {board1, board2, board3, board4, board5, board6, board7, board8, board9, board10};

//**************************************************************************************************************************************
float cm = 0; // universal variable to start timer

// variables to calculate datarate
float packetCount = 0;
float pm = 0;
float interval = 1000; // one second interval
float dataRate = 0;

// variables for postPing
String incomingMessage = "";
String postPing = "";
const char *PARAM_MESSAGE = "message";
int postCounter = 0;
float pm2 = 0;
float interval2 = 10;

float pm3 = 0;
float interval3 = 60000;

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

void setup()
{
  // initialise status led
  pinMode(ledPin1, OUTPUT);
  // Serial port for debugging purposes
  Serial.begin(115200);

  espNowSetup();
  wifiSetup();

  // print receiver's mac address
  Serial.println("\n");
  Serial.println(WiFi.macAddress());

  // Route for root / web page
  // also the get method for sending the sensor data
  //**************************************************************************************************************************************
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", combinedSensorData().c_str()); });
  //**************************************************************************************************************************************
  server.on("/values", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", dataFun().c_str()); });
  //**************************************************************************************************************************************
  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/message", HTTP_POST, [](AsyncWebServerRequest *request)
            {
        
        if (request->hasParam(PARAM_MESSAGE, true)) {
            incomingMessage = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            //incomingMessage = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + incomingMessage); });
  //**************************************************************************************************************************************
  // Start server
  server.begin();
}

int i = 0;
unsigned long int cm5 = 0;
unsigned long int pm5 = 0;
unsigned long int interval5 = 10;
void loop()
{
  cm = millis();
  dataRateDisplay();

  pingTest();      // pinging the ESP32 Wifi from the android app
  resettingTime(); // resets time in all esp 32 senders every 60 seconds

  cm5 = millis();

  if (cm - pm >= interval)
  {
    Serial.println(boardsStruct[myData.id].accelx[i]);
    i++;

    if (i > 9)
    {
      i = 0;
    }
  }
}

void resettingTime()
{

  if (cm - pm3 > interval3)
  {

    pm3 = cm;
    resetSig.time = 1;
    esp_err_t result = esp_now_send(0, (uint8_t *)&resetSig, sizeof(struct_signal));

    if (result == ESP_OK)
    {
      //Serial.println("Sent time with success");
    }
    else
    {
     // Serial.println("Error sending the data");
    }
  }
}

void pingTest()
{
  if ((cm - pm2) >= interval2)
  {
    pm2 = cm;
    //Serial.println(incomingMessage);

    if (incomingMessage != "")
    {
      postPing = "success";
      postCounter++;
    }
    if (postCounter >= 10)
    { // every 100 ms this resets message and postPing
      postPing = "";
      incomingMessage = "";
      postCounter = 0;
    }
  }
}
String dataFun()
{

  String temp0 = "{\n\"dataRate\":\"";
  String temp1 = String(dataRate);
  String temp2 = "\",\n";
  String temp3 = "\"postPing\":\"";
  String temp4 = String(postPing);
  String temp5 = "\",\n";

  // duplicate the following lines below for key value pairs
  String temp6 = "\"temp\":\"";
  String temp7 = String(0);
  String temp8 = "\"\n";

  // ending of the JSON object
  String temp100 = "}";
  String data = temp0 + temp1 + temp2 + temp3 + temp4 + temp5 + temp6 + temp7 + temp8 + temp100;
  return data;
}
void dataRateDisplay()
{

  if ((cm - pm) >= interval)
  {
    dataRate = (packetCount / (interval / 1000));
    //Serial.println(dataRate);

    pm = cm;
    packetCount = 0;
  }
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{

  char macStr[18];
  // Serial.print("Packet received from: ");
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  // Serial.printf("Board ID %u: %u bytes\n", myData.id, len);

  // incrementing packet count
  packetCount++;
  // Update the structures with the new incoming data
  boardsStruct[myData.id].id = myData.id;
  boardsStruct[myData.id].time = myData.time;

  // updating sensor data (need for loops for cases where packetSize >1)
  for (int i = 0; i < packetSize; i++)
  {
    boardsStruct[myData.id].accelx[i] = myData.accelx[i];
    boardsStruct[myData.id].accely[i] = myData.accely[i];
    boardsStruct[myData.id].accelz[i] = myData.accelz[i];
    boardsStruct[myData.id].gyrox[i] = myData.gyrox[i];
    boardsStruct[myData.id].gyroy[i] = myData.gyroy[i];
    boardsStruct[myData.id].gyroz[i] = myData.gyroz[i];
  }
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");

  if (status == ESP_NOW_SEND_SUCCESS)
  {
    //Serial.println("Delivery Success");
    toggleLED();
  }
  else
  {
    // turning off LED
    //Serial.println("Delivery Fail");
    ledStatus1 = false;
    digitalWrite(ledPin1, ledStatus1);
  }
}

void espNowSetup()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  // register peer
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // register peers

  for (int i = 0; i < devices; i++)
  {

    memcpy(peerInfo.peer_addr, broadcastAddresses[i], sizeof(uint8_t) * 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
      return;
    }
  }
}

// wifi setup for wifi transmission
void wifiSetup()
{
  // Connect to Wi-Fi
  WiFi.softAP(ssidAP, passwordAP);
  //Serial.println(WiFi.softAPIP());
}

// establishing function for data transmission through wifi (JSON Code here)
String combinedSensorData()
{

  String stat;
  for (int i = 0; i < devices; i++)
  {
    // int i=1;
    String c = "{\n";
    String d = "\"id\":\"" + String(i) + "\",\n";
    String e = "\"time\":\"" + String(boardsStruct[i].time) + "\",\n";
    String f = "\"accelx\":[";
    String g = "\"accely\":[";
    String h = "\"accelz\":[";
    String k = "\"gyrox\":[";
    String l = "\"gyroy\":[";
    String m = "\"gyroz\":[";

    String totalaccelx, totalaccely, totalaccelz, totalgyrox, totalgyroy, totalgyroz, y, y2;

    for (int j = 0; j < packetSize; j++)
    {
      // int j=0;
      String accelx = "\"" + String(boardsStruct[i].accelx[j]) + "\"";
      String accely = "\"" + String(boardsStruct[i].accely[j]) + "\"";
      String accelz = "\"" + String(boardsStruct[i].accelz[j]) + "\"";
      String gyrox = "\"" + String(boardsStruct[i].gyrox[j]) + "\"";
      String gyroy = "\"" + String(boardsStruct[i].gyroy[j]) + "\"";
      String gyroz = "\"" + String(boardsStruct[i].gyroz[j]) + "\"";

      if (j < packetSize - 1)
      {
        totalaccelx = totalaccelx + accelx + ",";
        totalaccely = totalaccely + accely + ",";
        totalaccelz = totalaccelz + accelz + ",";
        totalgyrox = totalgyrox + gyrox + ",";
        totalgyroy = totalgyroy + gyroy + ",";
        totalgyroz = totalgyroz + gyroz + ",";
      }
      else
      {
        totalaccelx = totalaccelx + accelx;
        totalaccely = totalaccely + accely;
        totalaccelz = totalaccelz + accelz;
        totalgyrox = totalgyrox + gyrox;
        totalgyroy = totalgyroy + gyroy;
        totalgyroz = totalgyroz + gyroz;
      }

      y = "],\n";
      y2 = "]\n}";
    }

    if (i < devices - 1)
    {
      stat = stat + c + d + e + f + totalaccelx + y + g + totalaccely + y + h + totalaccelz + y + k + totalgyrox + y + l + totalgyroy + y + m + totalgyroz + y2 + ",";
    }
    else
    {
      stat = stat + c + d + e + f + totalaccelx + y + g + totalaccely + y + h + totalaccelz + y + k + totalgyrox + y + l + totalgyroy + y + m + totalgyroz + y2;
    }
  }

  String a = "{\n";
  String b = "\"boards\":[";
  String z = "]\n}";

  String combinedText = a + b + stat + z;

  return combinedText;
}
