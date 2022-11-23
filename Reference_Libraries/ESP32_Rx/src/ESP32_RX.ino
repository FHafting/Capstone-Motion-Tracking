#include "headers.h"
//#include "functions.h"
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

// Replace with your network credentials
const char *ssidSTA = "CANADIAN WINTER BELL";
const char *passwordSTA = "Shivam1912__";

const char *ssidSTA2 = "iPhone (100)";
const char *passwordSTA2 = "shiv1912";

const char *ssidAP = "ESP32_Shiv";
const char *passwordAP = "12345678";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);



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


// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}
// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

uint8_t incomingData;
// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");
incomingData= *data;
}


void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  spiffsSetup();
  wifiSetup();
   WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html"); });
  server.on("/mpuVal", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", String(incomingData).c_str()); });

  // Start server
  server.begin();
}

void loop()
{

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
}
