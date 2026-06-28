#ifndef WIFI_RTC_H
#define WIFI_RTC_H

#include "astro_data.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <String.h>


DateTime sys_time;
String Time;


const char* ssid = "StarTracker";
const char* password = "Track@123";

WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

int WiFi_timeout = 20;  
int RTC_timeout = 5;    

bool WiFi_status = false;
bool timeUpdate = false;
float timeZoneOffset=5.5*3600;


String formatTime(DateTime dt) 
{
  char buffer[20];
  sprintf(buffer, "%02d-%02d-%04d %02d:%02d:%02d", dt.day(), dt.month(), dt.year(), dt.hour(), dt.minute(), dt.second());
  return String(buffer);
}

void connectWiFi() 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  long current_time = millis();

  Serial.print("\nConnecting to WiFi SSID: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    if (millis() - current_time > (WiFi_timeout * 1000)) 
    {
      Serial.println("\nWiFi not available"); 
      return;
    }
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    WiFi_status = true;
    Serial.println("\nWiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
    
}

void update_time() 
{
    rtc.begin();
   // DateTime rtc_time = rtc.now();

    if (WiFi.status() == WL_CONNECTED) {
        timeClient.begin();
        timeClient.setTimeOffset(timeZoneOffset);
        timeClient.update();

        unsigned long epochTime = timeClient.getEpochTime();
        sys_time = DateTime(epochTime); 

        Serial.print("Current Local Time: ");
        Serial.println(formatTime(sys_time));

        if (timeUpdate) 
        {  
          rtc.adjust(sys_time);
          Serial.println("RTC updated with local time!");
          timeUpdate = false;
        }
    }

    //rtc_time = rtc.now();
    //Time = formatTime(rtc_time);
   // Serial.println("RTC after update: " + Time);
}


#endif