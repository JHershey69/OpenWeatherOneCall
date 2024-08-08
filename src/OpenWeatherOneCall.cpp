/*
   OpenWeatherOneCall.cpp v4.0.0
   Updated for ArduinoJSON v7 on Aug 1, 2024
   copyright 2020/2024 - Jessica Hershey
   www.github.com/JHershey69

   Open Weather Map - Weather Conditions
   For ESP32 Only
   Viva La Resistance

   REVISION HISTORY
   See User Manual

   **************************************************************
   ************NO USER EDITABLE LINES IN THIS FILE***************
   **************************************************************
*/

#include "OpenWeatherOneCall.h"



void dateTimeConversion(long _epoch, char *_buffer, int _format);

OpenWeatherOneCall::OpenWeatherOneCall()
{

}

// For Normal Weather calls *************
#define DS_URL1 "https://api.openweathermap.org/data/3.0/onecall"
char DS_URL2[100];
#define DS_URL3 "&appid="

// For Air Quality calls current *************
#define AQ_URL1 "https://api.openweathermap.org/data/2.5/air_pollution?lat="
#define AQ_URL2 "&lon="
#define AQ_URL3 "&appid="

// For TIMESTAMP Weather Calls **********
#define TS_URL1 "https://api.openweathermap.org/data/3.0/onecall/timemachine"
#define TS_URL2 "&dt="

// For OVERVIEW weather calls ***********
#define OV_URL1 "https://api.openweathermap.org/data/3.0/onecall/overview"
#define OV_URL2 "&date="

// For CITY Id calls
#define CI_URL1 "api.openweathermap.org/data/3.0/weather?id="
#define CI_URL2 "&appid="

#define SIZEOF(a) sizeof(a)/sizeof(*a)


// Main Method for Weather API Call and Parsing
int OpenWeatherOneCall::parseWeather(void)
{
    OpenWeatherOneCall::setOpenWeatherKey(ONECALLKEY);
    OpenWeatherOneCall::setExcl(myEXCLUDES);
    OpenWeatherOneCall::setUnits(myUNITS);
    OpenWeatherOneCall::setDateTimeFormat(myDTF);
    OpenWeatherOneCall::setCurrent(myCURRENT);
    OpenWeatherOneCall::setAirQuality(myAIRQUALITY);
    OpenWeatherOneCall::setTimestamp(myTIMESTAMP, timestampDate);
    OpenWeatherOneCall::setOverview(myOVERVIEW, overviewDate);


    int error_code = 0;

    if (WiFi.status() != WL_CONNECTED)
        {
            return 25;
        }


    // LOCATION MODE SET

    if(locationMode == 1)
        {
            OpenWeatherOneCall::setLatLon(myLATITUDE, myLONGITUDE);
        }
    else if(locationMode == 2)
        {
            OpenWeatherOneCall::setLatLon(myCITYID);
        }
    else if(locationMode == 3)
        {
            OpenWeatherOneCall::setLatLon();
        }


    unsigned int SIZE_CAPACITY = 32768;

    // Bitwise function that sets the bits for the EXCLUDES argument
    SIZE_CAPACITY = OpenWeatherOneCall::setExcludes(USER_PARAM.OPEN_WEATHER_EXCLUDES);

    if((USER_PARAM.OPEN_WEATHER_LATITUDE) || (USER_PARAM.OPEN_WEATHER_LONGITUDE))
        {
            OpenWeatherOneCall::getLocationInfo();

            if(USER_PARAM.OPEN_WEATHER_TIMESTAMP)
                {
                    int error_code = OpenWeatherOneCall::createTimestamp();
                }
            else
                {
                    OpenWeatherOneCall::freeTimestampMem();
                }

            if(USER_PARAM.OPEN_WEATHER_OVERVIEW)
                {
                    int error_code = OpenWeatherOneCall::createOverview();
                }
            else
                {
                    OpenWeatherOneCall::freeOverviewMem();
                }

            if(USER_PARAM.OPEN_WEATHER_AIRQUALITY)
                {
                    int error_code =  OpenWeatherOneCall::createAQ(384);
                }


            if(USER_PARAM.OPEN_WEATHER_CURRENT)
                {
                    int error_code = OpenWeatherOneCall::createCurrent(SIZE_CAPACITY);
                }
            else
                {
                    OpenWeatherOneCall::freeCurrentMem();
                }



            if(error_code)
                {
                    return error_code;
                }
        }
    else
        {
            return 24; //Must set Latitude and Longitude somehow
        }

    return 0;
}
/*
REMOVED LEGACY CALLING METHOD AS OF 3.3.4
*/

int OpenWeatherOneCall::createTimestamp()
{
    int error_code = 0;

    char timestampURL[200] = {0};

    long tempEPOCH = extractDate(USER_PARAM.OPEN_WEATHER_TIMESTAMP_DATE);

    //Setup API URL for TIMESTAMP Weather Call [Single Timestamp only]

    sprintf(timestampURL,"%s?lat=%.6f&lon=%.6f%s%ld&units=%s%s%s",TS_URL1,USER_PARAM.OPEN_WEATHER_LATITUDE,USER_PARAM.OPEN_WEATHER_LONGITUDE,TS_URL2,tempEPOCH,units,DS_URL3,USER_PARAM.OPEN_WEATHER_DKEY);

    HTTPClient http;
    http.begin(timestampURL);
    int httpCode = http.GET();

    if (httpCode > 399)
        {
            if(httpCode == 401)
                {
                    http.end();
                    return 22;
                }
            else
                {
                    http.end();
                    return 21;
                }
        }


    JsonDocument doc;

    deserializeJson(doc, http.getString());

    strncpy(location.timezone,doc["timezone"],50);
    location.timezoneOffset = doc["timezone_offset"];

    if(!timestamp)
        {
            timestamp = (struct TIMESTAMP *)calloc(25,sizeof(struct TIMESTAMP));
        }

    //Current in historical is the time of the request on that day
    JsonObject current = doc["data"][0];
    timestamp[0].dayTime = current["dt"]; // 1607292481

    if(current["dt"])
        {
            // Create human readable date and time

            long tempTime = current["dt"];
            tempTime += location.timezoneOffset;
            dateTimeConversion(tempTime,timestamp[0].readableDateTime,USER_PARAM.OPEN_WEATHER_DATEFORMAT+9);
        }

    timestamp[0].sunrise = current["sunrise"]; // 1607256309

    if(current["sunrise"])
        {
            // Create human readable date and time

            long tempTime = current["sunrise"];
            tempTime += location.timezoneOffset;
            dateTimeConversion(tempTime,timestamp[0].readableSunrise,USER_PARAM.OPEN_WEATHER_DATEFORMAT+4);
        }

    timestamp[0].sunset = current["sunset"]; // 1607290280

    if(current["sunset"])
        {
            // Create human readable date and time

            long tempTime = current["sunset"];
            tempTime += location.timezoneOffset;
            dateTimeConversion(tempTime,timestamp[0].readableSunset,USER_PARAM.OPEN_WEATHER_DATEFORMAT+4);
        }

    timestamp[0].temperature = current["temp"]; // 35.82
    timestamp[0].apparentTemperature = current["feels_like"]; // 21.7
    timestamp[0].pressure = current["pressure"]; // 1010
    timestamp[0].humidity = current["humidity"]; // 51
    timestamp[0].dewPoint = current["dew_point"]; // 20.88
    timestamp[0].uvIndex = current["uvi"]; // 1.54
    timestamp[0].cloudCover = current["clouds"]; // 1
    timestamp[0].visibility = current["visibility"]; // 16093
    timestamp[0].windSpeed = current["wind_speed"]; // 16.11
    timestamp[0].windBearing = current["wind_deg"]; // 300
    timestamp[0].windGust = current["wind_gust"]; // 24.16

    // New rain and snow =======================
    if(current["rain"])
        {
            if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                {
                    float temp = current["rain"]["1h"];
                    timestamp[0].rainVolume = (temp/25.4); // 95
                }
            else
                timestamp[0].rainVolume = current["rain"]["1h"]; // 95

        }
    else
        timestamp[0].rainVolume = 0;



    if(current["snow"])
        {
            if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                {
                    float temp = current["snow"]["1h"];
                    timestamp[0].snowVolume = (temp/25.4); // 95
                }
            else
                timestamp[0].snowVolume = current["snow"]["1h"]; // 95

        }
    else
        timestamp[0].snowVolume = 0;



    JsonObject current_weather_0 = current["weather"][0];
    timestamp[0].id = current_weather_0["id"]; // 800

    timestamp[0].main = (char *)realloc(timestamp[0].main,sizeof(char) * strlen(current_weather_0["main"])+1);
    strncpy(timestamp[0].main,current_weather_0["main"],strlen(current_weather_0["main"])+1);

    timestamp[0].summary = (char *)realloc(timestamp[0].summary,sizeof(char) * strlen(current_weather_0["description"])+1);
    strncpy(timestamp[0].summary,current_weather_0["description"],strlen(current_weather_0["description"])+1);

    strncpy(timestamp[0].icon,current_weather_0["icon"],strlen(current_weather_0["icon"])+1);

    dateTimeConversion(timestamp[0].dayTime,timestamp[0].weekDayName,9);

    http.end();
    return 0;
}

int OpenWeatherOneCall::createCurrent(int sizeCap)
{

    char getURL[200] = {0};
    int alertz = 0;

    sprintf(getURL,"%s?lat=%.6f&lon=%.6f&lang=%s%s&units=%s%s%s",DS_URL1,USER_PARAM.OPEN_WEATHER_LATITUDE,USER_PARAM.OPEN_WEATHER_LONGITUDE,USER_PARAM.OPEN_WEATHER_LANGUAGE,DS_URL2,units,DS_URL3,USER_PARAM.OPEN_WEATHER_DKEY);
    // printf("\n%s\n",getURL);

    HTTPClient http;
    http.begin(getURL);
    int httpCode = http.GET();

    if (httpCode > 399)
        {
            if(httpCode == 401)
                {
                    http.end();
                    return 22;
                }

            http.end();
            return 21;

        }

    const size_t capacity = sizeCap;
    JsonDocument doc;
    deserializeJson(doc, http.getString());


    strncpy(location.timezone,doc["timezone"],50);
    location.timezoneOffset = doc["timezone_offset"];

    if(exclude.current)
        {
            OpenWeatherOneCall::freeCurrentMem();
        }
    else
        {
            if(!current)
                {
                    current = (struct nowData *)calloc(1,sizeof(struct nowData));
                    if(current == NULL)
                        {
                            return 23;
                        }
                }

            JsonObject currently = doc["current"];
            current->dayTime = currently["dt"]; // 1586781931

            if(currently["dt"])
                {
                    // Create human readable date and time

                    long tempTime = currently["dt"];
                    tempTime += location.timezoneOffset;
                    dateTimeConversion(tempTime,current->readableDateTime,USER_PARAM.OPEN_WEATHER_DATEFORMAT);
                    dateTimeConversion(tempTime,current->readableWeekdayName,9);
                }


            current->sunriseTime = currently["sunrise"]; // 1612267442

            if(currently["sunrise"])
                {
                    // Create human readable date and time

                    long tempTime = currently["sunrise"];
                    tempTime += location.timezoneOffset;
                    dateTimeConversion(tempTime,current->readableSunrise,USER_PARAM.OPEN_WEATHER_DATEFORMAT+4);
                }

            current->sunsetTime = currently["sunset"]; // 1612304218

            if(currently["sunset"])
                {
                    // Create human readable date and time

                    long tempTime = currently["sunset"];
                    tempTime += location.timezoneOffset;
                    dateTimeConversion(tempTime,current->readableSunset,USER_PARAM.OPEN_WEATHER_DATEFORMAT+4);
                }



            current->temperature = currently["temp"]; // 287.59
            current->apparentTemperature = currently["feels_like"]; // 281.42
            current->pressure = currently["pressure"]; // 1011
            current->humidity = currently["humidity"]; // 93
            current->dewPoint = currently["dew_point"]; // 286.47
            current->uvIndex = currently["uvi"]; // 6.31
            current->cloudCover = currently["clouds"]; // 90
            current->visibility = currently["visibility"]; // 8047
            current->windSpeed = currently["wind_speed"]; // 10.3
            current->windBearing = currently["wind_deg"]; // 170

            if(currently["wind_gust"])
                {
                    current->windGust = currently["wind_gust"];
                }

            if(currently["snow"]["1h"])
                {
                    if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                        {
                            float temp = currently["snow"]["1h"];
                            current->snowVolume = (temp/25.4); // 95
                        }
                    else
                        current->snowVolume = currently["snow"]["1h"]; // 95

                }
            else
                current->snowVolume = 0;

            if(currently["rain"]["1h"])
                {
                    if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                        {
                            float temp = currently["rain"]["1h"];
                            current->rainVolume = (temp/25.4); // 95
                        }
                    else
                        current->rainVolume = currently["rain"]["1h"]; // 95

                }
            else
                current->rainVolume = 0;




            current->id = currently["weather"][0]["id"];

            current->main = (char *)realloc(current->main,sizeof(char) * strlen(currently["weather"][0]["main"])+1);
            if(current->main == NULL)
                {
                    return 23;
                }
            strncpy(current->main,currently["weather"][0]["main"],strlen(currently["weather"][0]["main"])+1);

            current->summary = (char *)realloc(current->summary,sizeof(char) * strlen(currently["weather"][0]["description"])+1);
            if(current->summary == NULL)
                {
                    return 23;
                }
            strncpy(current->summary,currently["weather"][0]["description"],strlen(currently["weather"][0]["description"])+1);

            strncpy(current->icon,currently["weather"][0]["icon"],strlen(currently["weather"][0]["icon"])+1);
        }

    if(exclude.daily)
        {
            OpenWeatherOneCall::freeForecastMem();
        }
    else
        {
            if(!forecast)
                {
                    forecast = (struct futureData *)calloc(8,sizeof(struct futureData));
                    if(forecast == NULL)
                        {
                            return 23;
                        }
                }

            JsonArray daily = doc["daily"];
            for (int x = 0; x < 8; x++)
                {
                    forecast[x].dayTime = daily[x]["dt"]; // 1586793600

                    if(daily[x]["dt"])
                        {
                            long tempTime = daily[x]["dt"];
                            tempTime += location.timezoneOffset;
                            dateTimeConversion(tempTime,forecast[x].readableDateTime,USER_PARAM.OPEN_WEATHER_DATEFORMAT+9);
                        }

                    forecast[x].sunriseTime = daily[x]["sunrise"]; // 1586773262

                    if(daily[x]["sunrise"])
                        {
                            long tempTime = daily[x]["sunrise"];
                            tempTime += location.timezoneOffset;
                            dateTimeConversion(tempTime,forecast[x].readableSunrise,USER_PARAM.OPEN_WEATHER_DATEFORMAT+4);
                        }

                    forecast[x].sunsetTime = daily[x]["sunset"]; // 1586820773

                    if(daily[x]["sunset"])
                        {
                            long tempTime = daily[x]["sunset"];
                            tempTime += location.timezoneOffset;
                            dateTimeConversion(tempTime,forecast[x].readableSunset,USER_PARAM.OPEN_WEATHER_DATEFORMAT+4);
                        }

                    forecast[x].temperatureDay = daily[x]["temp"]["day"]; // 288.74
                    forecast[x].temperatureLow = daily[x]["temp"]["min"]; // 286.56
                    forecast[x].temperatureHigh = daily[x]["temp"]["max"]; // 293.23
                    forecast[x].temperatureNight = daily[x]["temp"]["night"]; // 286.56
                    forecast[x].temperatureEve = daily[x]["temp"]["eve"]; // 293.23
                    forecast[x].temperatureMorn = daily[x]["temp"]["morn"]; // 286.56

                    forecast[x].apparentTemperatureHigh = daily[x]["feels_like"]["day"]; // 280.11
                    forecast[x].apparentTemperatureLow = daily[x]["feels_like"]["night"]; // 280.29
                    forecast[x].apparentTemperatureEve = daily[x]["feels_like"]["eve"]; // 280.11
                    forecast[x].apparentTemperatureMorn = daily[x]["feels_like"]["morn"]; // 280.29

                    forecast[x].pressure = daily[x]["pressure"]; // 1006
                    forecast[x].humidity = daily[x]["humidity"]; // 91
                    forecast[x].dewPoint = daily[x]["dew_point"]; // 287.28
                    forecast[x].windSpeed = daily[x]["wind_speed"]; // 14.2

                    if(daily[x]["wind_gust"])
                        {
                            forecast[x].windGust = daily[x]["wind_gust"];
                        }


                    forecast[x].windBearing = daily[x]["wind_deg"]; // 180

                    forecast[x].id = daily[x]["weather"][0]["id"]; // 800

                    if(daily[x]["weather"][0]["main"])
                        {
                            forecast[x].main = (char *)realloc(forecast[x].main,sizeof(char) * strlen(daily[x]["weather"][0]["main"])+1);
                            if(forecast[x].main == NULL)
                                {
                                    return 23;
                                }
                            strncpy(forecast[x].main,daily[x]["weather"][0]["main"],strlen(daily[x]["weather"][0]["main"])+1);
                        }

                    if(daily[x]["weather"][0]["description"])
                        {
                            forecast[x].summary = (char *)realloc(forecast[x].summary,sizeof(char) * strlen(daily[x]["weather"][0]["description"])+1);
                            if(forecast[x].summary == NULL)
                                {
                                    return 23;
                                }
                            strncpy(forecast[x].summary,daily[x]["weather"][0]["description"],strlen(daily[x]["weather"][0]["description"])+1);
                        }

                    strncpy(forecast[x].icon,daily[x]["weather"][0]["icon"],strlen(daily[x]["weather"][0]["icon"])+1);

                    forecast[x].cloudCover = daily[x]["clouds"]; // 95
                    forecast[x].pop = daily[x]["pop"]; // 95

                    if(daily[x]["rain"])
                        {
                            if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                                {
                                    float temp = daily[x]["rain"];
                                    forecast[x].rainVolume = (temp/25.4); // 95
                                }
                            else
                                forecast[x].rainVolume = daily[x]["rain"]; // 95

                        }
                    else
                        forecast[x].rainVolume = 0; // 95


                    if(daily[x]["snow"])
                        {
                            if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                                {
                                    float temp = daily[x]["snow"];
                                    forecast[x].snowVolume = (temp/25.4); // 95
                                }
                            else
                                forecast[x].snowVolume = daily[x]["snow"]; // 95

                        }
                    else
                        forecast[x].snowVolume = 0;

                    forecast[x].uvIndex = daily[x]["uvi"]; // 6.31

                    dateTimeConversion(forecast[x].dayTime,forecast[x].weekDayName,9);
                }

        }

    if(exclude.alerts)
        {
            OpenWeatherOneCall::freeAlertMem();
        }
    else
        {
            if(doc["alerts"][0])
                {
                    //count alerts here
                    for(int z = 0; z < 10; z++)
                        {
                            if(doc["alerts"][z])
                                {
                                    MAX_NUM_ALERTS = z+1;
                                }
                        }


                    if(!alert);
                    {
                        alert = (struct ALERTS *)calloc(MAX_NUM_ALERTS,sizeof(struct ALERTS));
                        if(alert == NULL)
                            {
                                return 23;
                            }
                    }

                    //Start for loop of maximum alerts here
                    for(int x = 0; x < MAX_NUM_ALERTS; x++)
                        {

                            JsonObject ALERTS_0 = doc["alerts"][x];

                            if(ALERTS_0["sender_name"])
                                {
                                    alert[x].senderName = (char *)realloc(alert[x].senderName,sizeof(char) * strlen(ALERTS_0["sender_name"])+1);
                                    if(alert[x].senderName == NULL)
                                        {
                                            return 23;
                                        }
                                    strncpy(alert[x].senderName,ALERTS_0["sender_name"],strlen(ALERTS_0["sender_name"])+1);
                                }

                            if(ALERTS_0["event"])
                                {
                                    alert[x].event = (char *)realloc(alert[x].event,sizeof(char) * strlen(ALERTS_0["event"])+1);
                                    if(alert[x].event == NULL)
                                        {
                                            return 23;
                                        }
                                    strncpy(alert[x].event,ALERTS_0["event"],strlen(ALERTS_0["event"])+1);

                                }

                            if(ALERTS_0["start"])
                                {
                                    long tempTime = ALERTS_0["start"];
                                    alert[x].alertStart = tempTime;
                                    tempTime += location.timezoneOffset;
                                    dateTimeConversion(tempTime,alert[x].startInfo,USER_PARAM.OPEN_WEATHER_DATEFORMAT);
                                }

                            if(ALERTS_0["end"])
                                {
                                    long tempTime = ALERTS_0["end"];
                                    alert[x].alertEnd = tempTime;
                                    tempTime += location.timezoneOffset;
                                    dateTimeConversion(tempTime,alert[x].endInfo,USER_PARAM.OPEN_WEATHER_DATEFORMAT);
                                }

                            if(ALERTS_0["description"])
                                {
                                    alert[x].summary = (char *)realloc(alert[x].summary,sizeof(char) * strlen(ALERTS_0["description"])+1);
                                    if(alert[x].summary == NULL)
                                        {
                                            return 23;
                                        }
                                    strncpy(alert[x].summary,ALERTS_0["description"],strlen(ALERTS_0["description"])+1);
                                }
                        } //end for

                }
            else
                {
                    // If alerts are not excluded but there are none, reset NUM_MAX_ALERTS and MEMORY
                    MAX_NUM_ALERTS = 0;
                    OpenWeatherOneCall::freeAlertMem();
                }
        }


    if(exclude.hourly)
        {
            OpenWeatherOneCall::freeHourMem();
        }
    else
        {
            if(doc["hourly"])
                {
                    if(!hour)
                        {
                            hour = (struct HOURLY *)calloc(48, sizeof(struct HOURLY));
                            if(hour == NULL)
                                {
                                    return 23;
                                }
                        }

                    JsonArray hourly = doc["hourly"];
                    for(int h = 0; h < 48; h++)
                        {

                            JsonObject hourly_0 = hourly[h];
                            //hour[h].dayTime = hourly_0["dt"]; // 1604336400

                            if(hourly_0["dt"])
                                {
                                    long tempTime = hourly_0["dt"];
                                    hour[h].dayTime = tempTime;
                                    //tempTime += location.timezoneOffset;
                                    dateTimeConversion(tempTime,hour[h].readableTime,6);
                                }

                            hour[h].temperature = hourly_0["temp"]; // 46.58
                            hour[h].apparentTemperature = hourly_0["feels_like"]; // 28.54
                            hour[h].pressure = hourly_0["pressure"]; // 1015
                            hour[h].humidity = hourly_0["humidity"]; // 31
                            hour[h].dewPoint = hourly_0["dew_point"]; // 19.2
                            hour[h].cloudCover = hourly_0["clouds"]; // 20
                            hour[h].visibility = hourly_0["visibility"]; // 10000
                            hour[h].windSpeed = hourly_0["wind_speed"]; // 22.77
                            hour[h].windBearing = hourly_0["wind_deg"]; // 300


                            if(hourly_0["snow"])
                                {
                                    if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                                        {
                                            float temp = hourly_0["snow"];
                                            hour[h].snowVolume = (temp/25.4); // 95
                                        }
                                    else
                                        hour[h].snowVolume = hourly_0["snow"]; // 95

                                }
                            else
                                hour[h].snowVolume = 0;


                            if(hourly_0["rain"])
                                {
                                    if(USER_PARAM.OPEN_WEATHER_UNITS == 2)
                                        {
                                            float temp = hourly_0["rain"];
                                            hour[h].rainVolume = (temp/25.4); // 95
                                        }
                                    else
                                        hour[h].rainVolume = hourly_0["rain"]; // 95

                                }
                            else
                                hour[h].rainVolume = 0;


                            JsonObject hourly_0_weather_0 = hourly_0["weather"][0];
                            hour[h].id = hourly_0_weather_0["id"]; // 801

                            if(hourly_0_weather_0["main"])
                                {
                                    hour[h].main = (char *)realloc(hour[h].main,sizeof(char) * strlen(hourly_0_weather_0["main"])+1);
                                    if(hour[h].main == NULL)
                                        {
                                            return 23;
                                        }
                                    strncpy(hour[h].main,hourly_0_weather_0["main"],strlen(hourly_0_weather_0["main"])+1);
                                }

                            if(hourly_0_weather_0["description"])
                                {
                                    hour[h].summary = (char *)realloc(hour[h].summary,sizeof(char) * strlen(hourly_0_weather_0["description"])+1);
                                    if(hour[h].summary == NULL)
                                        {
                                            return 23;
                                        }
                                    strncpy(hour[h].summary,hourly_0_weather_0["description"],strlen(hourly_0_weather_0["description"])+1);
                                }


                            strncpy(hour[h].icon,hourly_0_weather_0["icon"],strlen(hourly_0_weather_0["icon"])+1);

                            hour[h].pop = hourly_0["pop"]; // 0
                        }
                }
        }


    if(exclude.minutely)
        {
            OpenWeatherOneCall::freeMinuteMem();
        }
    else
        {
            if(doc["minutely"])
                {
                    if(!minute)
                        {
                            minute = (struct MINUTELY *)calloc(61, sizeof(struct MINUTELY));
                            if(minute == NULL)
                                {
                                    return 23;
                                }
                        }


                    JsonArray minutely = doc["minutely"];

                    for(int x = 0; x<61; x++)
                        {
                            minute[x].dayTime = minutely[x]["dt"];
                            long tempTime = minutely[x]["dt"];
                            //tempTime += -3600;
                            dateTimeConversion(tempTime,minute[x].readableTime,USER_PARAM.OPEN_WEATHER_DATEFORMAT+4);

                            minute[x].precipitation = minutely[x]["precipitation"]; // 0
                        }
                }
        }

    http.end();
    return 0;
}

int OpenWeatherOneCall::createOverview()
{

    // NTP Server
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = myTimeZone * 3600;
    const int   daylightOffset_sec = 3600;
    char timeString[11];
    int maxRetries  = 10;

    // Initialize NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Get current time
    struct tm timeinfo;

    int retries = 0;
    while (!getLocalTime(&timeinfo) && retries < maxRetries)
        {
            Serial.println("Failed to obtain time, retrying...");
            delay(2000); // Wait 2 seconds before retrying
            retries++;
        }
    if (retries == maxRetries)
        {
            Serial.println("Failed to obtain time after maximum retries");
            return false;
        }
    Serial.println("Time obtained successfully");

    if(strcmp(overviewDate,"TODAY")==0)
        {
            strftime(timeString, sizeof(timeString), "%Y-%m-%d", &timeinfo);
        }
    else if(strcmp(overviewDate,"TOMORROW")==0)
        {
            // Add 1 to the day
            timeinfo.tm_mday += 1;

            // Normalize the time structure (this handles overflow of days, months, etc.)
            mktime(&timeinfo);
            strftime(timeString, sizeof(timeString), "%Y-%m-%d", &timeinfo);
        }

    char overviewURL[200] = {0};

    sprintf(overviewURL,"%s?lat=%.6f&lon=%.6f%s%s&units=%s%s%s",OV_URL1,USER_PARAM.OPEN_WEATHER_LATITUDE,USER_PARAM.OPEN_WEATHER_LONGITUDE,OV_URL2,timeString,OV_units,DS_URL3,USER_PARAM.OPEN_WEATHER_DKEY);

    //printf("\n%s\n",overviewURL);

    HTTPClient http;
    http.begin(overviewURL);
    int httpCode = http.GET();

    if (httpCode > 399)
        {
            if(httpCode == 401)
                {
                    http.end();
                    return 22;
                }
            else
                {
                    http.end();
                    return 21;
                }
        }

    if(!overView)
        {
            overView = (struct OVERVIEW *)calloc(1,sizeof(struct OVERVIEW));
            if(overView == NULL)
                {
                    return 23;
                }
        }
    // String input;

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, http.getString());

    if (error)
        {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return 22;
        }


    double lat = doc["lat"]; // 39.953701
    double lon = doc["lon"]; // -74.197899
    const char* tz = doc["tz"]; // "-04:00"
    const char* date = doc["date"]; // "2024-08-06"
    const char* units = doc["units"]; // "imperial"
    const char* weather_overview = doc["weather_overview"]; // "The current weather in our area is overcast ..


    overView->lat = doc["lat"]; // 39.953701
    overView->lon = doc["lon"]; // -74.197899
    overView->tz = strdup(doc["tz"]); // "-04:00"
    overView->date = strdup(doc["date"]); // "2024-08-06"
    overView->units = strdup(doc["units"]); // "imperial"
    overView->weather_overview = strdup(doc["weather_overview"]);


    http.end();
    return 0;
}

void OpenWeatherOneCall::allocateAndCopy(char** destination, const char* source)
{
    *destination = (char*)malloc(strlen(source) + 1); // Allocate memory
    if (*destination != NULL)
        {
            strcpy(*destination, source); // Copy the string
        }
}

int OpenWeatherOneCall::setLatLon(float _LAT, float _LON)
{
    int error_code = 0;

    if(abs(_LAT) <= 90)
        {
            USER_PARAM.OPEN_WEATHER_LATITUDE = _LAT;
            location.LATITUDE = _LAT; //User copy
        }
    else
        error_code += 1;

    if(abs(_LON) <= 180)
        {
            USER_PARAM.OPEN_WEATHER_LONGITUDE = _LON;
            location.LONGITUDE = _LON; //User copy
        }
    else
        error_code += 2;

    if(error_code)
        return error_code;

    return (EXIT_SUCCESS);
}

int OpenWeatherOneCall::setLatLon(int _CITY_ID)
{
    int error_code = 0;

    char cityURL[110];
    char* URL1 = "http://api.openweathermap.org/data/3.0/weather?id=";
    char* URL2 = "&appid=";

    sprintf(cityURL,"%s%d%s%s",URL1,_CITY_ID,URL2,USER_PARAM.OPEN_WEATHER_DKEY);
    error_code = OpenWeatherOneCall::parseCityCoordinates(cityURL);
    if(error_code)
        return error_code;

    return (EXIT_SUCCESS);

}

int OpenWeatherOneCall::setLatLon(void)
{
    // IP address of NON-CELLULAR WiFi. Hotspots won't work properly.

    int error_code = 0;
    error_code = OpenWeatherOneCall::getIPLocation();
    if(error_code)
        {
            return error_code;
        }

    error_code = OpenWeatherOneCall::getIPAPILocation(_ipapiURL);
    if(error_code)
        {
            return error_code;
        }

    return 0;
}


int OpenWeatherOneCall::parseCityCoordinates(char* CTY_URL)
{
    int error_code = 0;

    HTTPClient http;
    http.begin(CTY_URL);
    int httpCode = http.GET();
    if(httpCode > 399)
        {
            if(httpCode == 404)
                {
                    http.end();
                    error_code += 4;
                    return error_code;
                }
            else
                {
                    http.end();
                    error_code += 5;
                    return error_code;
                }

        }

    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 270;
    JsonDocument doc;

    deserializeJson(doc, http.getString());

    if(doc["coord"]["lon"])
        {
            USER_PARAM.OPEN_WEATHER_LONGITUDE = doc["coord"]["lon"]; // -74.2
        }
    else
        {
            error_code += 6;
        }

    if(doc["coord"]["lat"])
        {
            USER_PARAM.OPEN_WEATHER_LATITUDE = doc["coord"]["lat"]; // 39.95
        }
    else
        {
            error_code += 7;
        }

    http.end();

    if(error_code)
        {
            http.end();
            return error_code;
        }

    return (EXIT_SUCCESS);
}

int OpenWeatherOneCall::getIPLocation()
{
    int error_code = 0;

    HTTPClient http;
    http.begin("https://api64.ipify.org/ HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n");

    int httpCode = http.GET();

    if(httpCode > 399)
        {
            if(httpCode == 404)
                {
                    http.end();
                    error_code += 8;
                    return error_code;
                }
            else
                {
                    http.end();
                    error_code += 9;
                    return error_code;
                }

        }

    int streamSize = http.getSize();
    char stringVarout[streamSize+1];

    String stringVarin = http.getString();
    strcpy(stringVarout,stringVarin.c_str());

    http.end();

    strncpy(_ipapiURL,"https://ipapi.co/",38);
    strcat(_ipapiURL,stringVarout);
    strcat(_ipapiURL,"/json/");

    return 0;
}


int OpenWeatherOneCall::getIPAPILocation(char* URL)
{
    int error_code = 0;
    HTTPClient http;
    http.begin(URL);
    int ipapi_httpCode = http.GET();

    if(ipapi_httpCode > 399)
        {
            if(ipapi_httpCode == 404)
                {
                    error_code += 10;
                }
            else
                {
                    error_code += 11;
                }
            http.end();
            return error_code;
        }


    const size_t capacity = JSON_OBJECT_SIZE(26) + 490;
    JsonDocument doc;

    deserializeJson(doc, http.getString());

    strncpy(location.CITY,doc["city"],60); // "Lakewood"
    strncpy(location.STATE,doc["region_code"],2); // "NJ"
    strncpy(location.COUNTRY,doc["country_code"],3); // "US"
    USER_PARAM.OPEN_WEATHER_LATITUDE = doc["latitude"]; // 40.0881
    location.LATITUDE = doc["latitude"];
    USER_PARAM.OPEN_WEATHER_LONGITUDE = doc["longitude"]; // -74.1963
    location.LONGITUDE = doc["longitude"];

    http.end();

    return 0;
}


void OpenWeatherOneCall::initAPI(void)
{
    memset(USER_PARAM.OPEN_WEATHER_DKEY,NULL,1);
    USER_PARAM.OPEN_WEATHER_LATITUDE = 0.0;
    USER_PARAM.OPEN_WEATHER_LONGITUDE = 0.0;
    USER_PARAM.OPEN_WEATHER_UNITS = 2;
    USER_PARAM.OPEN_WEATHER_EXCLUDES = 0;
    USER_PARAM.OPEN_WEATHER_TIMESTAMP = 0;
    strcpy(USER_PARAM.OPEN_WEATHER_TIMESTAMP_DATE,"01/01/1979");
    USER_PARAM.OPEN_WEATHER_OVERVIEW = 0;
    strcpy(USER_PARAM.OPEN_WEATHER_OVERVIEW_DATE,"0000-00-00");


}


int OpenWeatherOneCall::getLocationInfo()
{
    int error_code = 0;

    char locationURL[200];

    sprintf(locationURL,"https://api.bigdatacloud.net/data/reverse-geocode-client/?latitude=%f&longitude=%f",USER_PARAM.OPEN_WEATHER_LATITUDE,USER_PARAM.OPEN_WEATHER_LONGITUDE);

    HTTPClient http;
    http.begin(locationURL);

    int httpCode = http.GET();
    if (httpCode > 399)
        {
            if(httpCode == 401)
                {
                    error_code = 17;
                }
            else if(httpCode == 404)
                {
                    error_code = 18;
                }
            else
                error_code = 19;

            http.end();
            return error_code;
        }


    JsonDocument doc;

    deserializeJson(doc, http.getString());

    if(doc["locality"])
        {
            strncpy(location.CITY,doc["locality"],60);
        }
    else
        {
            http.end();
            return 20;
        }

    if(doc["principalSubdivisionCode"])
        {
            strncpy(location.STATE,doc["principalSubdivisionCode"],10);
        }
    else
        {
            http.end();
            return 20;
        }

    if(doc["countryCode"])
        {
            strncpy(location.COUNTRY,doc["countryCode"],10);
        }
    else
        {
            http.end();
            return 20;
        }


    http.end();

    return 0;
}



int OpenWeatherOneCall::createAQ(int sizeCap)
{
    char getURL[200] = {0};

    sprintf(getURL,"%s%.6f%s%.6f%s%s",AQ_URL1,USER_PARAM.OPEN_WEATHER_LATITUDE,AQ_URL2,USER_PARAM.OPEN_WEATHER_LONGITUDE,AQ_URL3,USER_PARAM.OPEN_WEATHER_DKEY);

    HTTPClient http;
    http.begin(getURL);
    int httpCode = http.GET();

    if (httpCode > 399)
        {
            if(httpCode == 401)
                {
                    http.end();
                    return 22;
                }

            http.end();
            return 21;

        }

    const size_t capacity = sizeCap;
    JsonDocument doc;
    deserializeJson(doc, http.getString());


    quality = (struct airQuality *)calloc(1,sizeof(struct airQuality));

    float coord_lon = doc["coord"]["lon"]; // -74.1975
    float coord_lat = doc["coord"]["lat"]; // 39.9533

    JsonObject list_0 = doc["list"][0];

    quality -> aqi = list_0["main"]["aqi"]; // 2

    JsonObject list_0_components = list_0["components"];
    quality -> co = list_0_components["co"]; // 230.31
    quality -> no = list_0_components["no"]; // 0.43
    quality -> no2 = list_0_components["no2"]; // 1.69
    quality -> o3 = list_0_components["o3"]; // 100.14
    quality -> so2 = list_0_components["so2"]; // 0.88
    quality -> pm2_5 = list_0_components["pm2_5"]; // 0.76
    quality -> pm10 = list_0_components["pm10"]; // 1.04
    quality -> nh3 = list_0_components["nh3"]; // 0.43

    quality -> dayTime = list_0["dt"]; // 1615838400

    http.end();
    return 0;


}


int OpenWeatherOneCall::setExcludes(int EXCL)
{
    unsigned int excludeMemSize = 32768;
    // Allocate MEM requirements for parser ****************************
    unsigned int EXCL_SIZES[] = {1010,3084,15934,4351,607}; //Alerts, Minutely, Hourly, Daily, Currently
    // Name the exclude options *********************
    char* EXCL_NAMES[] = {"current", "daily", "hourly", "minutely", "alerts"};

    exclude.all_excludes = EXCL; //<- Sets the individual bits to 1 for excludes
    strcpy(DS_URL2,"&exclude=");

    int SIZE_X = (sizeof(EXCL_NAMES) / sizeof(EXCL_NAMES[0]));

    // Required to insert comma properly *****************************
    int comma_pos = exclude.all_excludes;
    int comma_sub = 16;


    // Add exclude names to URL and insert a comma *******************
    for (int x = SIZE_X-1; x >= 0; x--)
        {
            if (exclude.all_excludes >> x & 1)
                {
                    strcat(DS_URL2,EXCL_NAMES[x]);

                    if ((x == 4) && (comma_pos > 16))
                        {
                            strcat(DS_URL2,",");
                            comma_pos -= comma_sub;
                        }
                    if ((x == 3) && ((comma_pos > 8)&&(comma_pos < 16)))
                        {
                            strcat(DS_URL2,",");
                            comma_pos -= comma_sub;
                        }

                    if ((x == 2) && ((comma_pos > 4)&&(comma_pos < 8)))
                        {
                            strcat(DS_URL2,",");
                            comma_pos -= comma_sub;
                        }

                    if ((x == 1) && ((comma_pos > 2)&&(comma_pos < 4)))
                        {
                            strcat(DS_URL2,",");
                            comma_pos -= comma_sub;
                        }


                    if (x < (sizeof(EXCL_NAMES) / sizeof(EXCL_NAMES[0])) - 1)
                        {
                            excludeMemSize -= EXCL_SIZES[x];
                        }
                }
            comma_sub = comma_sub/2;
        }

    return excludeMemSize;
}

int OpenWeatherOneCall::setOpenWeatherKey(char* owKey)
{
    int error_code = 0;
    if((strlen(owKey) < 25) || (strlen(owKey) > 64))
        {
            return 12;
        };

    strncpy(USER_PARAM.OPEN_WEATHER_DKEY,owKey,100);

    return 0;
}

int OpenWeatherOneCall::setExcl(int _EXCL)
{
    if((_EXCL > 31) || (_EXCL < 0))
        {
            USER_PARAM.OPEN_WEATHER_EXCLUDES = NULL;
            return 14;
        }
    else
        USER_PARAM.OPEN_WEATHER_EXCLUDES = _EXCL;

    return 0;
}

int OpenWeatherOneCall::setUnits(int _UNIT)
{
    if(_UNIT > 4)
        {
            return 15;
        }
    USER_PARAM.OPEN_WEATHER_UNITS = _UNIT;

    switch(_UNIT)
        {
        case 3:
            strcpy(units,"STANDARD");
            strcpy(OV_units,"standard");
            break;
        case 2:
            strcpy(units,"IMPERIAL");
            strcpy(OV_units,"imperial");
            break;
        case 1:
            strcpy(units,"METRIC");
            strcpy(OV_units,"metric");
            break;
        default :
            strcpy(units,"IMPERIAL");
            strcpy(OV_units,"imperial");
        }

    return 0;
}

int OpenWeatherOneCall::setCurrent(int _CUR)
{

    USER_PARAM.OPEN_WEATHER_CURRENT = _CUR;

    return 0;
}

int OpenWeatherOneCall::setAirQuality(int _AQ)
{

    USER_PARAM.OPEN_WEATHER_AIRQUALITY = _AQ;

    return 0;
}

int OpenWeatherOneCall::setTimestamp(int _HIS, const char* _DATE)
{

    USER_PARAM.OPEN_WEATHER_TIMESTAMP = _HIS;
    strcpy(USER_PARAM.OPEN_WEATHER_TIMESTAMP_DATE,_DATE);

    return 0;
}

int OpenWeatherOneCall::setOverview(int _OVR, const char* _DATE)
{

    USER_PARAM.OPEN_WEATHER_OVERVIEW = _OVR;
    strcpy(USER_PARAM.OPEN_WEATHER_OVERVIEW_DATE,_DATE);

    return 0;
}

int OpenWeatherOneCall::setDateTimeFormat(int _DTF)
{
    if(_DTF > 4)
        return 16;
    USER_PARAM.OPEN_WEATHER_DATEFORMAT = _DTF;
    return 0;
}

// Takes any Human Readable date converts to epoch
long OpenWeatherOneCall::extractDate(char str[11])
{

    long epochH;

    struct tm t;
    time_t t_of_day;

    char m[3];
    char d[3];
    char yr[5];


    for(int x=0; x<10; x++)
        {
            if(isDigit(str[x]))
                {
                    if(x<2)
                        {
                            m[x] = str[x];
                        }
                    else
                        {
                            if(x<5)
                                {
                                    d[x-3] = str[x];
                                }
                            else
                                {
                                    if(x<10)
                                        {
                                            yr[x-6] = str[x];
                                        }
                                }
                        }
                }
        }

    String month = String(m);
    String day = String(d);
    String year = String(yr);


    t.tm_year = year.toInt()-1900;  // Year - 1900
    t.tm_mon = month.toInt() - 1;           // Month, where 0 = jan
    t.tm_mday = day.toInt();          // Day of the month
    t.tm_hour = 00;
    t.tm_min = 00;
    t.tm_sec = 00;
    t.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);


    return (long) t_of_day;

}



// free routines

void OpenWeatherOneCall::freeCurrentMem(void)
{
    if(current)
        {
            free(current->summary);
            current->summary = NULL;
            free(current->main);
            current->main = NULL;
            free(current);
            current = NULL;
        }

}

void OpenWeatherOneCall::freeForecastMem(void)
{
    if(forecast)
        {
            for( int x = 8; x > 0; x--)
                {
                    free(forecast[x-1].summary);
                    free(forecast[x-1].main);
                }
            free(forecast);
            forecast[0].summary = NULL;
            forecast[0].main = NULL;
            forecast = NULL;
        }

}

void OpenWeatherOneCall::freeTimestampMem(void)
{
    if(timestamp)
        {
            for( int x = 24; x > 0; x--)
                {
                    free(timestamp[x-1].summary);
                    free(timestamp[x-1].main);
                }
            free(timestamp);
            timestamp[0].summary = NULL;
            timestamp[0].main = NULL;
            timestamp = NULL;
        }

}

void OpenWeatherOneCall::freeAlertMem(void)
{
    if(alert)
        {
            for( int x = MAX_NUM_ALERTS; x > 0; x--)
                {
                    free(alert[x].senderName);
                    free(alert[x].event);
                    free(alert[x].summary);
                }
            free(alert);
            alert[0].senderName = NULL;
            alert[0].event = NULL;
            alert[0].summary = NULL;
            alert = NULL;
        }
}

void OpenWeatherOneCall::freeHourMem(void)
{
    if(hour)
        {
            for( int x = 48; x > 0; x--)
                {
                    free(hour[x-1].summary);
                    free(hour[x-1].main);
                }
            free(hour);
            hour[0].summary = NULL;
            hour[0].main = NULL;
            hour = NULL;
        }
}

void OpenWeatherOneCall::freeMinuteMem(void)
{
    if(minute)
        {
            free(minute);
            minute = NULL;
        }
}

void OpenWeatherOneCall::freeQualityMem(void)
{
    if(quality)
        {
            free(quality);
            quality = NULL;
        }
}

void OpenWeatherOneCall::freeOverviewMem(void)
{
    if (overView)
        {
            free(overView);
            overView = NULL;
        }
}

char* OpenWeatherOneCall::getErrorMsgs(int _errMsg)
{
    if((_errMsg > SIZEOF(errorMsgs)) || (_errMsg < 1))
        return "Error Number Out of RANGE";
    strcpy_P(buffer, (char*)pgm_read_dword(&(errorMsgs[_errMsg - 1])));
    return buffer;
}


OpenWeatherOneCall::~OpenWeatherOneCall()
{

    OpenWeatherOneCall::freeCurrentMem();
    OpenWeatherOneCall::freeForecastMem();
    OpenWeatherOneCall::freeAlertMem();
    OpenWeatherOneCall::freeHourMem();
    OpenWeatherOneCall::freeMinuteMem();
    OpenWeatherOneCall::freeTimestampMem();
    OpenWeatherOneCall::freeQualityMem();
    OpenWeatherOneCall::freeOverviewMem();

}

// Looks like this is the end
