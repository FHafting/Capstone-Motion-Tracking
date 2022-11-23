#include <Arduino.h>

#include <WiFi.h>
#include "time.h"

const char *ssid = "CANADIAN WINTER BELL";
const char *password = "Shivam1912__";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -18000;
const int daylightOffset_sec = 0;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
int interval = 10;
int counter = 0;
struct tm timeinfo;
int temp = 0;
int temp2 = 0;
int sec1 = 0;
int sec2 = 0;

void setup()
{
  Serial.begin(115200);

  // connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // disconnect WiFi as it's no longer needed
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void loop()
{
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return;
    }
    if (temp == 0)
    {
      sec2 = timeinfo.tm_sec + 1;
      temp = 1;
    }
    sec1 = timeinfo.tm_sec;
    if (sec1 == sec2 && temp2 == 0)
    {
      counter = 0;
      temp2 = 1;
    }
    else
    {
      Serial.printf("%d:%d:%d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      if (counter < 10)
      {
        Serial.printf("00");
      }
      else if (counter < 100)
      {
        Serial.printf("0");
      }
      Serial.printf("%d\n", counter);

      counter = counter + interval;
      if (counter == 1000)
      {
        counter = 0;
      }
      previousMillis = currentMillis;
    }
  }
}