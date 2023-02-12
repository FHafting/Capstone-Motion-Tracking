#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

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
int temp = 0;
int temp2 = 0;
int temp3 = 0;
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
}

char time1[50];
char temp4[10];
unsigned long int timeFromServer;
unsigned long int timeFromMcu;
struct tm timeinfo;
struct tm timeinfo2;
unsigned long int drift;
int i = 0;

void loop()
{

  currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    i = i + 1;

    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return;
    }
    // time alignment with the MCU counter (to count 10 ms increments between the time pulled from the server)
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
    if(temp2==1)
    {

      counter = counter + interval;
      if (counter == 1000)
      {
        counter = 0;
      }
      timeFromServer = (timeinfo.tm_min * 60 + timeinfo.tm_sec) * 1000 + counter;
      
      if (i==5000 && temp3 == 0)
      {
        timeFromMcu = timeFromServer;
        temp3 = 1;
      }
    }

    if (temp3 == 1)
    {
      timeFromMcu = timeFromMcu + interval;
      drift = timeFromServer - timeFromMcu;
      Serial.println(drift);
    }
    
  }
}
