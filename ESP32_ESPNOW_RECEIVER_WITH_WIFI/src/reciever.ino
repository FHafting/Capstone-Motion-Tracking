#include "headers.h"
// #include "functions.h"

const char *ssidAP = "ESP32_Shiv";
const char *passwordAP = "12345678";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const int packetSize = 1; // amount of sensor readings being sent within each packet
// Must match the sender structure

// initializing esp now
typedef struct struct_message
{
  int id;
  boolean status;
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

const int devices = 6; // must add boards below and in boardsStruct

// Creating upto 6 esp 32 devices
struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;
struct_message board5;
struct_message board6;

// Create an array with all the structures
struct_message boardsStruct[devices] = {board1, board2, board3, board4, board5, board6};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id].id = myData.id;
  boardsStruct[myData.id].time = myData.time;
  boardsStruct[myData.id].status = myData.status;

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
}

// wifi setup for wifi transmission
void wifiSetup()
{
  // Connect to Wi-Fi
  WiFi.softAP(ssidAP, passwordAP);
  Serial.println(WiFi.softAPIP());
}

// establishing functions for data transmission
String combinedSensorData()
{

  String test = "";
  return test;
}

String testData2()
{
  String test = "{\"ESP32 ID\":1,\n\"Sensor ID\":1,\n\"Time\":\"10:0000\",\n\"Value\": 12453.6987}";
  return test;
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  // initialize state of boards (only required to execute once)
  for (int i = 0; i < devices; i++)
  {
    boardsStruct[i].status = false;
  }

  wifiSetup();
  espNowSetup();

  // print receiver's mac address
  Serial.println(WiFi.macAddress());

  // Route for root / web page
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", combinedSensorData().c_str()); });

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Post route"); });

  // Start server
  server.begin();
}

void loop()
{


}
