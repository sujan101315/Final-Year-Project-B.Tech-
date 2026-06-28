#ifndef ASTRO_DATA_H
#define ASTRO_DATA_H

#include "Stepper.h"
#include <RTClib.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Wire.h>

RTC_DS3231 rtc;

int utcOffsetHours = 5;   // Change this to your local UTC offset (hours)
int utcOffsetMinutes = 30; // Change this to minutes offset (if applicable)


String objectName = "Sun"; // Default object

float temperature = 27.0, latitude = 22.0506247, longitude = 88.0717563, elevation = 16;
//float temperature = 27.0, latitude = 29.23, longitude = 79.50, elevation = 446;

String URL = "https://ssd.jpl.nasa.gov/api/horizons.api?format=json&COMMAND='";

bool API_status=false;
bool timeFound = false;
float Az = 0, preAz=0, Alt = 0, preAlt=0;
float apparentRA = 0, apparentDEC = 0;

String ra = "0h0ms", dec = "0°0'0\"";

struct objects {
    String object;
    int identifier;
};

objects data[] = {
    {"Sun", 10},
    {"Mercury", 199},
    {"Venus", 299},
    {"Mars", 499},
    {"Jupiter", 599},
    {"Saturn", 699},
    {"Uranus", 799},
    {"Neptune", 899},
    {"Pluto", 999},
    {"Moon", 301},
    {"Phobos", 401},
    {"Deimos", 402},
    {"Io", 501},
    {"Europa", 502},
    {"Ganymede", 503},
    {"Callisto", 504},
    {"Titan", 606},
    {"Enceladus", 610},
    {"Triton", 801},
    {"Ceres", 2000001},
    {"Vesta", 2000004},
    {"Eros", 2000433},
    {"Halley", 1000001},
    {"Bennu", 2101955},
    {"Voyager 1", 65531},
    {"Voyager 2", 65532},
    {"New Horizons", 65598},
    {"JWST", 65570}
};

uint16_t findObjectID(String name) {
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        if (data[i].object.equals(name)) {
            return data[i].identifier;
        }
    }
    return 0; // Return 0 if not found
}

String fetchHorizonsData(String apiUrl) {
    HTTPClient http;
    http.begin(apiUrl);
    int httpCode = http.GET();
    String payload = "NA";
    if (httpCode == HTTP_CODE_OK) 
    {
        payload = http.getString();
        API_status=true;
    }
    else
      API_status=false;
    http.end();
    return payload;
}

void parseData(String data) 
{
  //Serial.println(data);
  int startIndex = data.indexOf("$$SOE");
  if (startIndex == -1) 
  {
    Serial.println("Data line not found!");
    return;
  }
  
  int endIndex = data.indexOf("/", startIndex);
  if (endIndex == -1) 
  {
    Serial.println("Invalid endIndex, aborting.");
    return;
  }
  
  String dataLine = data.substring(startIndex, endIndex);

  int moonHorizon=0;
  if(data.indexOf("*m")||data.indexOf("m"))  ;
  int tokensInc = (moonHorizon != -1) ? 1 : 0;

  Serial.println("moonHorizon:");
  Serial.println(moonHorizon);

  Serial.println("Data Line:");
  Serial.println(dataLine);

  char dataBuffer[dataLine.length() + 1]; 
  strcpy(dataBuffer, dataLine.c_str()); 

  char *tokens = strtok(dataBuffer, " ");
  int count = 0;
  float raH = 0, raM = 0, raS = 0, decD = 0, decM = 0, decS = 0;
  String raStr, decStr;

  while (tokens != NULL) 
  {
    if (count == 9 + tokensInc) 
    {
      raH = atof(tokens);
      raStr = tokens;
      raStr += "h";
    } 
    else if (count == 10 + tokensInc) 
    {
      raM = atof(tokens);
      raStr += tokens;
      raStr += "m";
    } 
    else if (count == 11 + tokensInc) 
    {
      raS = atof(tokens);
      raStr += tokens;
      raStr += "s";
    } 
    else if (count == 12 + tokensInc) 
    {
      decD = atof(tokens);
      decStr += tokens;
      decStr += "°";
    } 
    else if (count == 13 + tokensInc) 
    {
      decM = atof(tokens);
      decStr += tokens;
      decStr += "'";
    } 
    else if (count == 14 + tokensInc) 
    {
      decS = atof(tokens);
      decStr += tokens;
      decStr += "\"";
    } 
    else if (count == 17 + tokensInc) 
    {
      Az = atof(tokens);
    } 
    else if (count == 18 + tokensInc) 
    {
      Alt = atof(tokens);
    }

    tokens = strtok(NULL, " ");
    count++;

    if (count > 20) 
    {
      Serial.println("Error: Unexpected token count.");
      break;
    }
  }

  ra = raStr;
  dec = decStr;

  Serial.printf("RA: %d %d %d \t RA: %s", int(raH), int(raM), int(raS), raStr.c_str());
  Serial.println();
  Serial.printf("DEC: %d %d %d \t DEC: %s", int(decD), int(decM), int(decS), decStr.c_str());
  Serial.println("\n");

  apparentRA = (raH + raM / 60.0 + raS / 3600.0) * 15.0;
  if (apparentRA >= 360) apparentRA -= 360;

  apparentDEC = decD + (decD < 0 ? -(decM / 60.0 + decS / 3600.0) : (decM / 60.0 + decS / 3600.0));
}

void fetchAPI(String obj) 
{   
    int ID= findObjectID(obj);
    Serial.printf("Object to track: %s\tID: %d\n", obj.c_str(), ID );

    String apiUrl = URL + String(ID) + "'&OBJ_DATA='YES'&MAKE_EPHEM='YES'&EPHEM_TYPE='OBSERVER'&CENTER='coord'&SITE_COORD='";
    apiUrl += String(longitude, 2) + "," + String(latitude, 2) + "," + String(elevation) + "'&START_TIME='";

    DateTime localTime = rtc.now(); // Get local time from RTC
    DateTime astro_time = localTime - TimeSpan(0, utcOffsetHours, utcOffsetMinutes, 0); // Convert to UTC
    DateTime stop_time_obj = astro_time + TimeSpan(0, 0, 1, 0); // Adds 1 minute

    char start_time[20], stop_time[20];
    snprintf(start_time, sizeof(start_time), "%d-%02d-%02d %02d:%02d", astro_time.year(), astro_time.month(), astro_time.day(), astro_time.hour(), astro_time.minute());
    snprintf(stop_time, sizeof(stop_time), "%d-%02d-%02d %02d:%02d", stop_time_obj.year(), stop_time_obj.month(), stop_time_obj.day(), stop_time_obj.hour(), stop_time_obj.minute());

    Serial.printf("Current UTC Time: %d-%02d-%02d %02d:%02d\n", astro_time.year(), astro_time.month(), astro_time.day(), astro_time.hour(), astro_time.minute());


    apiUrl += String(start_time) + " '&STOP_TIME='" + String(stop_time) + "'&STEP_SIZE='1d'";

    Serial.printf("URL: %s\n", apiUrl.c_str());

    String payload = fetchHorizonsData(apiUrl);
    if (payload != "NA") 
    {
        parseData(payload);
        Serial.print("Apparent RA: "); Serial.println(apparentRA, 6);
        Serial.print("Apparent DEC: "); Serial.println(apparentDEC, 6);
        Serial.print("Azimuth: "); Serial.println(Az, 6);
        Serial.print("Altitude: "); Serial.println(Alt, 6);
    } else {
        Serial.println("Error fetching data");
    }
}

#endif
