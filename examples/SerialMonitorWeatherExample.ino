/*
   Open Weather One Call Library
   Copyright 2020 - Jessica Hershey
   www.github.com/JHershey69
   
   One Call API key at www.openweathermap.org

   SerialMonitorWeatherExample.ino
   
   Returns CURRENT and 7 DAY FORECAST
   
   Variables accessed as OWOC.current.temperature or OWOC.forecast[x].temperature (where x is day 0-6)
*/

//WiFi Connection required
#include <WiFi.h>

//Required libraries
#include <ArduinoJson.h>
#include "HTTPClient.h"
#include <OpenWeatherOneCall.h>

//==================================================

#define HOMESSID "USE YOUR INFO HERE"
#define HOMEPW "USE YOUR INFO HERE"

#define ONECALLKEY "USE KEY FROM OPENWEATHERMAP DOT ORG"
#define GOOGLEKEY "USE KEY FROM GOOGLE DEVELOPER FOR GEOLOCATION"


// Los Angeles
float myLatitude = 34.0522; <-----------------------------in range to use GPS coordinates
float myLongitude = 118.2437;

bool metric = false; //<------------------------------TRUE is METRIC, FALSE is IMPERIAL, BLANK is KELVIN


OpenWeatherOneCall OWOC; // <--------------------------- Invoke Library

void connectWifi() {

  WiFi.begin(HOMESSID, HOMEPW);

  Serial.print("Connecting.");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.print("Connected to: ");
  Serial.println(HOMESSID);


} //======================== END WIFI CONNECT =======================


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("Serial Monitor Initialized");

  while (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }


  //=================================
  // Get the Weather Forecast
  //=================================

  // Examples of calls:
  // For manual coordinates or GPS
  // OWOC.parseWeather(ONECALLKEY, NULL, myLatitude, myLongitude, metric,NULL)
  // For GEOLOCATION BASED ON WiFi
  // OWOC.parseWeather(ONECALLKEY, GOOGLEKEY, NULL, NULL, metric,NULL)
  // For call by City ID (not active as of v1.11 for future use)
  // OWOC.parseWeather(ONECALLKEY, NULL, NULL, NULL, metric,city_id)
   
  OWOC.parseWeather(ONECALLKEY, GOOGLEKEY, myLatitude, myLongitude, metric,city_id); //<---- V1.11 added city_id parameter


//=================================================
// Print 7 day High Temp results
//=================================================

  Serial.println(OWOC.current.temperature);

  Serial.println("");
  Serial.println("7 Day High Temperature Forecast:");

  for (int y = 0; y < (sizeof(OWOC.forecast) / sizeof(OWOC.forecast[0])) - 1; y++) {
    //Date from epoch forecast[y].dayTime
    long DoW = OWOC.forecast[y].dayTime / 86400L;
    int day_of_week = (DoW + 4) % 7;

    Serial.print(OWOC.short_names[day_of_week]);
    Serial.print(": ");
    Serial.println(OWOC.forecast[y].temperatureHigh);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
