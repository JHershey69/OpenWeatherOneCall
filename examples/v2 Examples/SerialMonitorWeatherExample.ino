
/*
   Open Weather One Call Library
   v1.2.0
   Copyright 2020 - Jessica Hershey
   www.github.com/JHershey69

   One Call API key at www.openweathermap.org

   SerialMonitorWeatherExample.ino

   Returns CURRENT and 7 DAY FORECAST

   Variables accessed as OWOC.current.temperature or OWOC.forecast[x].temperature (where x is day 0-6)

   All print commands in this example are to the Serial Monitor
*/

//WiFi Connection required
#include <WiFi.h>

//Required libraries
#include <ArduinoJson.h>
#include "HTTPClient.h"
#include <OpenWeatherOneCall.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//==================================================

#define HOMESSID "YOUR SSID IDENTIFIER"
#define HOMEPW "YOUR SSID PASSWORD"

#define ONECALLKEY "YOUR ONE CALL WEATHER MAP API KEY"
#define GOOGLEKEY "USE KEY FROM GOOGLE DEVELOPER FOR GEOLOCATION"

// Sample City IDs **********************************

int city_id = 4504476; //<------------ Toms River, NJ
//int city_id = 4164138; // <------------ Miami, FL
//int city_id = 5128581; // <------------ New York City, NY
//int city_id = 1697175; // <------------- Olongapo, PH
//int city_id = 4142224; // <------------- Delaware, USA


// Sample Lat and Lon for Los Angeles
float myLatitude = 34.0522; //<-----------------------------in range to use GPS coordinates
float myLongitude = 118.2437;

bool metric = false; //<------------------------------TRUE is METRIC, FALSE is IMPERIAL, BLANK is KELVIN


OpenWeatherOneCall OWOC; // <--------------------------- Invoke Library like this

void connectWifi() {

  WiFi.begin(HOMESSID, HOMEPW);

  Serial.print("Connecting.");

  int TryNum = 0;

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
    TryNum++;
    if (TryNum > 20) {
      Serial.println("Unable to connect check your parameters.");
      for (;;);
    }
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
  // OWOC.parseWeather(ONECALLKEY, NULL, myLatitude, myLongitude, excludes, metric, NULL,EXCLUDES,HISTORICAL)

  // For GEOLOCATION BASED ON WiFi
  // OWOC.parseWeather(ONECALLKEY, GOOGLEKEY, NULL, NULL, excludes, metric, NULL, EXCLUDES, HISTORICAL)

  // For call by City ID (active as of v1.2.0)
  // OWOC.parseWeather(ONECALLKEY, NULL, NULL, NULL, excludes, metric, city_id, EXCLUDES, HISTORICAL)

  /*
     EXCLUDES ARE:
     EXCL_C(urrent) EXCL_D(aily) EXCL_H(ourly) EXCL_M(inutely) EXCL_A(lerts)
     In the form EXCL_C+EXCL_D+EXCL_H etc
     NULL is EXCLUDE NONE (Send ALL Info)
  */

  /*
     Historical Data is for the last 1-5 days
     NULL returns NO HISTORICAL DATA
     HISTORICAL Data overrides Current and is the ONLY data returned
     Change the below argument to number of days prior desired - 24 hours returned
     So, if today is Friday and you want Wednesday use 2
  */

int  HISTORICAL = 1; //<--------------------- set to # days for Historical weather, NULL for current

  OWOC.parseWeather(ONECALLKEY, NULL, NULL, NULL, metric, city_id, EXCL_H + EXCL_M, HISTORICAL); //<---------excludes hourly, minutely, historical data 1 day



  if (!HISTORICAL)
  {

    //=================================================
    // Print 7 day High Temp results to Serial Monitor
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

    Serial.println("");
    Serial.println("**********ALERTS********");
    Serial.print("From: ");
    Serial.println(OWOC.alert[0].sender_name);
    Serial.print("Event: ");
    Serial.println(OWOC.alert[0].event);
  }else{
    Serial.print("Historical Temperature for DATE requested: ");
    Serial.println(OWOC.history[0].temp);
    Serial.print("00:00 Hour Historical Temperature for DATE requested: ");
    Serial.println(OWOC.history[1].temp);
    Serial.print("01:00 Hour Historical Temperature for DATE requested: ");
    Serial.println(OWOC.history[2].temp);
    
  }
  /*
    Serial.println("**********MINUTELY Precipitation**************");

    for (int y = 0;y<61;y++)
    {
    Serial.print("Minute ");
    Serial.print((y));
    Serial.print(": ");
    Serial.println(OWOC.minute[y].precipitation);
    }
  */
}

void loop() {
  // put your main code here, to run repeatedly:

}
