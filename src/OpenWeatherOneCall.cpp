/*
   OpenWeatherOneCall.cpp v1.2.0 (Added new routine for City ID usage)
   copyright 2020 - Jessica Hershey
   www.github.com/JHershey69

   Open Weather Seven Day Forecast Library
   Seven Day and Current Weather Conditions
   Viva La Resistance
*/

#include "Arduino.h"            // Basic Arduino Library
#include "OpenWeatherOneCall.h"
#include <HTTPClient.h>         // Required
#include <ArduinoJson.h>        // Version 6 Required
#include "NAComplete.h"          // Must include for City ID search North America ONLY CA/US/MX

OpenWeatherOneCall::OpenWeatherOneCall()
{

}


#define DS_URL1 "https://api.openweathermap.org/data/2.5/onecall"
#define DS_URL2 "&units="
#define DS_URL3 "&appid="

#define GEOLOCATIONURL "https://www.googleapis.com/geolocation/v1/geolocate"



String short_names[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};


int OpenWeatherOneCall::getCoordinates(String googleKey)
{

    HTTPClient http;

    String jsonSend;    // This will be the json we send to Google with nearby MAC ADDRESSES

    // Scan for nearby WiFi networks

    int numNetworks = WiFi.scanNetworks();

    /*
     * Add 3 nearest WiFi MAC Addresses in this JSON form:
      {
      "wifiAccessPoints": [
      "macAddress":"4A:5D:36:35:BA:6D",
      "macAddress":"48:5D:36:35:BA:6C",
      "macAddress":"B0:B9:8A:EF:D2:D8"
      ]
      }
    */

    // Creating the json with some string operations

    jsonSend = "{\n";
    jsonSend += "\"wifiAccessPoints\": [\n";
    for (int j = 0; j < numNetworks; ++j)
    {
        jsonSend += "{\n";
        jsonSend += "\"macAddress\" : \"";
        jsonSend += (WiFi.BSSIDstr(j));
        jsonSend += "\",\n";
        jsonSend += "\"signalStrength\": ";
        jsonSend += WiFi.RSSI(j);
        jsonSend += "\n";
        if (j < numNetworks - 1)
        {
            jsonSend += "},\n";
        }
        else
        {
            jsonSend += "}\n";
        }
    }
    jsonSend += ("]\n");
    jsonSend += ("}\n");


    // Setting up the url for the POST

    String geoLocURL = GEOLOCATIONURL;
    geoLocURL.concat("?key=");
    geoLocURL.concat(googleKey);

    // Lets send the json in the POST and get back an answer with getString(),
    // add header for application/json

    http.begin(geoLocURL);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(jsonSend);

    if (httpCode > 399)
    {
        return httpCode;

    }
    // Setting the parsing routine

    const size_t capacity = 2 * JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument geo(capacity);


    deserializeJson(geo, http.getString());     // parsing the return from Google into geo Document

    // Variables in parse

    latitude = geo["location"]["lat"];
    longitude = geo["location"]["lng"];

    // int accuracy = geo["accuracy"]; // 37

    http.end();
    return 0;
} // End of Coordinates Routine




int OpenWeatherOneCall::parseWeather(char* DKEY, char* GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS, int CITY_ID)
{

    // Clear the struct for current weather
    memset(&current, 0, sizeof current);

    char getURL[200] = {0};

    // F/C/K units

    if(SET_UNITS)
    {
        strcpy(units,"metric");
    }
    else
        strcpy(units,"imperial");


    HTTPClient http;


    if((!GKEY)&&(!CITY_ID))
    {
        sprintf(getURL,"%s?lat=%.6f&lon=%.6f%s%s%s%s",DS_URL1,SEEK_LATITUDE,SEEK_LONGITUDE,DS_URL2,units,DS_URL3,DKEY);
    }
    else if (CITY_ID)
    {

        // Binary search the database ht[TABLE_SIZE]
        int first = 1;
        int last = TABLE_SIZE-1;
        int middle = (first + last)/2;

        while(first <= last)
        {
            if(ht[middle].city_code < CITY_ID)
            {
                first = middle+1;
            }
            else if ( ht[middle].city_code == CITY_ID)
            {
                SEEK_LATITUDE = ht[middle].lat;
                SEEK_LONGITUDE = ht[middle].lon;
                sprintf(getURL,"%s?lat=%.6f&lon=%.6f%s%s%s%s",DS_URL1,SEEK_LATITUDE,SEEK_LONGITUDE,DS_URL2,units,DS_URL3,DKEY);
                break;
            }
            else
                last = middle -1;
            middle = (first+last)/2;
        }

        if(first > last)
        {
            SEEK_LATITUDE = ht[last].lat;
            SEEK_LONGITUDE = ht[last].lon;
            sprintf(getURL,"%s?lat=%.6f&lon=%.6f%s%s%s%s",DS_URL1,SEEK_LATITUDE,SEEK_LONGITUDE,DS_URL2,units,DS_URL3,DKEY);

        }

    }
    else
    {
        OpenWeatherOneCall::getCoordinates(GKEY);
        sprintf(getURL,"%s?lat=%.6f&lon=%.6f%s%s%s%s",DS_URL1,latitude,longitude,DS_URL2,units,DS_URL3,DKEY);
    }

    http.begin(getURL);
    int httpCode = http.GET();

    if (httpCode > 399)
    {
        return httpCode;
    }

    const size_t capacity = 56 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(8) + JSON_ARRAY_SIZE(48) + 14 * JSON_OBJECT_SIZE(1) + 66 * JSON_OBJECT_SIZE(4) + 9 * JSON_OBJECT_SIZE(6) + 35 * JSON_OBJECT_SIZE(10) + 13 * JSON_OBJECT_SIZE(11) + 4 * JSON_OBJECT_SIZE(13) + 4 * JSON_OBJECT_SIZE(14) + JSON_OBJECT_SIZE(16) + 9190;

    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, http.getString()); // <<---------------- - Streams JSON

    JsonObject currently = doc["current"];
    current.dayTime = currently["dt"]; // 1586781931
    current.temperature = currently["temp"]; // 287.59
    current.apparentTemperature = currently["feels_like"]; // 281.42
    current.pressure = currently["pressure"]; // 1011
    current.humidity = currently["humidity"]; // 93
    current.dewPoint = currently["dew_point"]; // 286.47
    current.uvIndex = currently["uvi"]; // 6.31
    current.cloudCover = currently["clouds"]; // 90
    current.visibility = currently["visibility"]; // 8047
    current.windSpeed = currently["wind_speed"]; // 10.3
    current.windBearing = currently["wind_deg"]; // 170
    current.windGust = currently["wind_gust"]; // 14.4
    current.icon = currently["weather"][0]["main"];


//===================================
// FOR loop to fill the 8 day struct
//===================================


    JsonArray daily = doc["daily"];
// JsonObject daily_0 = daily[0];


    for (int x = 0; x < (sizeof(forecast) / sizeof(forecast[0])) - 1; x++)
    {
        forecast[x].dayTime = daily[x]["dt"]; // 1586793600
        forecast[x].sunriseTime = daily[x]["sunrise"]; // 1586773262
        forecast[x].sunsetTime = daily[x]["sunset"]; // 1586820773

        forecast[x].temperatureHigh = daily[x]["temp"]["day"]; // 288.74
        forecast[x].temperatureMin = daily[x]["temp"]["min"]; // 286.56
        forecast[x].temperatureMax = daily[x]["temp"]["max"]; // 293.23
        forecast[x].temperatureLow = daily[x]["temp"]["night"]; // 286.56

        forecast[x].apparentTemperatureHigh = daily[x]["feels_like"]["day"]; // 280.11
        forecast[x].apparentTemperatureLow = daily[x]["feels_like"]["night"]; // 280.29

        forecast[x].pressure = daily[x]["pressure"]; // 1006
        forecast[x].humidity = daily[x]["humidity"]; // 91
        forecast[x].dewPoint = daily[x]["dew_point"]; // 287.28
        forecast[x].windSpeed = daily[x]["wind_speed"]; // 14.2
        forecast[x].windBearing = daily[x]["wind_deg"]; // 180

        forecast[x].precipType = daily[x]["weather"][0]["main"]; // "Rain"
        forecast[x].icon = daily[x]["weather"][0]["icon"]; // "10d"

        forecast[x].cloudCover = daily[x]["clouds"]; // 95
        forecast[x].precipIntensity = daily[x]["rain"]; // 23.31
        forecast[x].uvIndex = daily[x]["uvi"]; // 6.31
    }


//*************************END OF FOR LOOP
//****************************************



    http.end();
    return 0;
}
