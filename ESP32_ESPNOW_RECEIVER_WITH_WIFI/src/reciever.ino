#include "headers.h"
// #include "functions.h"

const char *ssidAP = "ESP32_Shiv";
const char *passwordAP = "12345678";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const int packetSize = 10; // amount of sensor readings being sent within each packet
// Must match the sender structure

//variables to calculate datarate
float packetCount = 0;
float cm = 0;
float pm = 0;
float interval =1000; //one second interval 
float dataRate =0;
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

const int devices = 10; // must add boards below and in boardsStruct

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

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  
  char macStr[18];
 // Serial.print("Packet received from: ");
 // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
 // Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
 // Serial.printf("Board ID %u: %u bytes\n", myData.id, len);

  //incrementing packet count
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
    

  if(i<devices-1){
    stat = stat + c + d + e + f + totalaccelx + y + g + totalaccely + y + h + totalaccelz + y + k + totalgyrox + y + l + totalgyroy + y + m + totalgyroz + y2 + ",";
  }
  else{
    stat = stat + c + d + e + f + totalaccelx + y + g + totalaccely + y + h + totalaccelz + y + k + totalgyrox + y + l + totalgyroy + y + m + totalgyroz + y2;
  }

  }

  String a = "{\n";
  String b = "\"boards\":[";
  String z = "]\n}";

  String combinedText = a + b + stat + z;

  return combinedText;
}


String dataFun(){
  String data = String(dataRate);
  return data;
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);


  espNowSetup();
  wifiSetup();

  // print receiver's mac address
  Serial.println("\n");
  Serial.println(WiFi.macAddress());

  // Route for root / web page
  //also the get method for sending the sensor data
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", combinedSensorData().c_str()); });
 server.on("/datarate", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", dataFun().c_str()); });
  // server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
  //{ request->send(200, "text/plain", "Post route"); });

  // Start server
  server.begin();
}



void loop()
{
  cm=millis();
  if ((cm - pm) >= interval)
  {
dataRate = (packetCount/(interval/1000));
    Serial.println(dataRate);
    pm=cm;
    packetCount=0;
  }

}


