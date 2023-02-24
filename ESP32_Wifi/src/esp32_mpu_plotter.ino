#include "headers.h"
// #include "functions.h"

const char *ssidAP = "ESP32_Shiv";
const char *passwordAP = "12345678";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const int packetSize = 10; // amount of sensor readings being sent within each packet
// Must match the sender structure

// initializing esp now
typedef struct struct_message
{
  int id;
  String time;
  int accelx[packetSize];
  int accely[packetSize];
  int accelz[packetSize];
  int gyrox[packetSize];
  int gyroy[packetSize];
  int gyroz[packetSize];
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

// Create an array with all the structures
struct_message boardsStruct[6] = {board1, board2, board3, board4, board5, board6};

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
  boardsStruct[myData.id - 1].time = myData.time;
  Serial.printf("time value: %d \n", boardsStruct[myData.id - 1].time);
  for (int i = 0; i++; i < packetSize)
  {
    boardsStruct[myData.id - 1].accelx[i] = myData.accelx[i];
    Serial.printf("accelx value%d: %d \n", i, boardsStruct[myData.id - 1].accelx[i]);
    boardsStruct[myData.id - 1].accely[i] = myData.accely[i];
    boardsStruct[myData.id - 1].accelz[i] = myData.accelz[i];
    boardsStruct[myData.id - 1].gyrox[i] = myData.gyrox[i];
    boardsStruct[myData.id - 1].gyroy[i] = myData.gyroy[i];
    boardsStruct[myData.id - 1].gyroz[i] = myData.gyroz[i];
  }

  Serial.println();
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
String testData1()
{
  String test = "Hey this is test set #1";
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

  wifiSetup();
  espNowSetup();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", testData1().c_str()); });
  server.on("/test2", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", testData2().c_str()); });

  // Start server
  server.begin();
}

void loop()
{

  // Acess the variables for each board
  /*int board1X = boardsStruct[0].x;
  int board1Y = boardsStruct[0].y;
  int board2X = boardsStruct[1].x;
  int board2Y = boardsStruct[1].y;
  int board3X = boardsStruct[2].x;
  int board3Y = boardsStruct[2].y;*/
}
