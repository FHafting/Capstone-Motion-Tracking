#include "headers.h"
// #include "functions.h"

// Replace with your network credentials
const char *ssidSTA = "CANADIAN WINTER BELL";
const char *passwordSTA = "Shivam1912__";

const char *ssidAP = "ESP32_Shiv";
const char *passwordAP = "12345678";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void wifiSetup()
{
  // Connect to Wi-Fi
  int connectionMode = 1;

  if (connectionMode == 1)
  {
    WiFi.softAP(ssidAP, passwordAP);
    Serial.println(WiFi.softAPIP());
  }
  else
  {

    WiFi.begin(ssidSTA, passwordSTA);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(100);
      Serial.println("Connecting to WiFi..");
    }
    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());
  }

  
}


//establishing functions for data transmission

String testData1(){

String test = "Hey this is test set #1";

return test;
}

String testData2(){

String test = {" \"userId\":1,\n \"id\":1,\n \"title\":\"delectus aut autem\",\n \"completed\": false\n "};

return test;
}


void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  wifiSetup();
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

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
}
