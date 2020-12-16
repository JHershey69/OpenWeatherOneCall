/*
   OpenWeatherOneCall.cpp v3.0.0
   copyright 2020 - Jessica Hershey
   www.github.com/JHershey69

   Open Weather Map - Weather Conditions
   For ESP32 Only
   Viva La Resistance

   REVISION HISTORY
   1.2.0 - Added CITY ID option
   1.3.0 - Added exclude values for API call
   2.0.0 - Added HISTORICAL WEATHER, uses UNIX EPOCH TIMESTAMP see DOCs
   2.0.1 - Fixed Historical Data bug for EPOCH calculation
   2.0.2 - Added units (Kelvin/Metric/Imperial) to Historical
   2.1.0 - Added TIMEZONE and OFFSET, removed unused vars left over from Dark Sky
   3.0.0 - Completely reworked the library - If something is broken revert to previous version and let me know
*/

#include "OpenWeatherOneCall.h"

OpenWeatherOneCall::OpenWeatherOneCall()
{

}

// For Normal Weather calls *************
#define DS_URL1 "https://api.openweathermap.org/data/2.5/onecall"
char DS_URL2[100];
#define DS_URL3 "&appid="

// For Historical Weather Calls **********
#define TS_URL1 "https://api.openweathermap.org/data/2.5/onecall/timemachine"
#define TS_URL2 "&dt="

// For CITY Id calls
#define CI_URL1 "api.openweathermap.org/data/2.5/weather?id="
#define CI_URL2 "&appid="

// Main Method for Weather API Call and Parsing
int OpenWeatherOneCall::parseWeather(void)
{
    int error_code = 0;
    unsigned int SIZE_CAPACITY = 32768;

    // Bitwise function that sets the bits for the EXCLUDES argument
    SIZE_CAPACITY = OpenWeatherOneCall::setExcludes(USER_PARAM.OPEN_WEATHER_EXCLUDES);

    if((USER_PARAM.OPEN_WEATHER_LATITUDE) || (USER_PARAM.OPEN_WEATHER_LONGITUDE))
        {

            OpenWeatherOneCall::getLocationInfo();

            if(USER_PARAM.OPEN_WEATHER_HISTORY)  //If Historical Weather is requested, no CURRENT weather returned
                {
                    OpenWeatherOneCall::freeCurrentMem();
                    OpenWeatherOneCall::freeForecastMem();
                    OpenWeatherOneCall::freeAlertMem();
                    OpenWeatherOneCall::freeHourMem();
                    OpenWeatherOneCall::freeMinuteMem();

                    int error_code = OpenWeatherOneCall::createHistory();
                }
            else
                {
                    int error_code = OpenWeatherOneCall::createCurrent(SIZE_CAPACITY);
                }
            if(error_code)
                {
                    return error_code;
                }
            //endif lat/lon
        }
    else
        {
            return 26; //Must set Latitude and Longitude somehow
        }

    return (EXIT_SUCCESS);
}

int OpenWeatherOneCall::parseWeather(char* DKEY, char* GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS, int CITY_ID, int API_EXCLUDES, int GET_HISTORY)
{
    //Legacy calling Method for versions prior to v3.0.0

    OpenWeatherOneCall::setOpenWeatherKey(DKEY);
    if(SET_UNITS){
            OpenWeatherOneCall::setUnits(METRIC);
    }
    OpenWeatherOneCall::setExcl(API_EXCLUDES);
    OpenWeatherOneCall::setHistory(GET_HISTORY);
    if(SEEK_LATITUDE && SEEK_LONGITUDE){
            OpenWeatherOneCall:setLatLon(SEEK_LATITUDE,SEEK_LONGITUDE);
    }else if (CITY_ID){
            OpenWeatherOneCall::setLatLon(CITY_ID);
    }else OpenWeatherOneCall::setLatLon();
    OpenWeatherOneCall::parseWeather();

}

int OpenWeatherOneCall::setLatLon(float _LAT, float _LON)
{
    int error_code = 0;

    if(abs(_LAT) <= 90)
        {
            USER_PARAM.OPEN_WEATHER_LATITUDE = _LAT;
            location.LATITUDE = _LAT; //User copy
        }
    else if(_LAT != NULL)
        error_code += 1;

    if(abs(_LON) <= 180)
        {
            USER_PARAM.OPEN_WEATHER_LONGITUDE = _LON;
            location.LONGITUDE = _LON; //User copy
        }
    else if(_LON != NULL)
        error_code += 2;

    if(error_code)
        return error_code;

    return (EXIT_SUCCESS);

}

int OpenWeatherOneCall::setLatLon(int _CITY_ID)
{

    int error_code = 0;

    char cityURL[110];
    char* URL1 = "http://api.openweathermap.org/data/2.5/weather?id=";
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
    DynamicJsonDocument doc(capacity);

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
    DynamicJsonDocument doc(capacity);

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
    strncpy(USER_PARAM.OPEN_WEATHER_DKEY,NULL,100);
    USER_PARAM.OPEN_WEATHER_LATITUDE = 0.0;
    USER_PARAM.OPEN_WEATHER_LONGITUDE = 0.0;
    USER_PARAM.OPEN_WEATHER_UNITS = 2;
    USER_PARAM.OPEN_WEATHER_EXCLUDES = 0;
    USER_PARAM.OPEN_WEATHER_HISTORY = 0;
}


int OpenWeatherOneCall::getLocationInfo()
{
    int error_code = 0;

    char locationURL[200];

    sprintf(locationURL,"https://api.bigdatacloud.net/data/reverse-geocode-client/?latitude=%f&longitude=%f",USER_PARAM.OPEN_WEATHER_LATITUDE,USER_PARAM.OPEN_WEATHER_LONGITUDE);

    HTTPClient http;
    http.begin(locationURL);             //<------------ Connect to OpenWeatherMap

    int httpCode = http.GET();
    if (httpCode > 399) //<- Check for connect errors
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


    const size_t capacity = JSON_ARRAY_SIZE(4) + JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 3*JSON_OBJECT_SIZE(6) + 2*JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(14) + 1050;
    DynamicJsonDocument doc(capacity);

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
            return 21;
        }

    if(doc["countryCode"])
        {
            strncpy(location.COUNTRY,doc["countryCode"],10);
        }
    else
        {
            http.end();
            return 22;
        }


    http.end();

    return 0;

}


int OpenWeatherOneCall::createHistory()
{
    int error_code = 0;
    char historyURL[200] = {0};

    if(USER_PARAM.OPEN_WEATHER_HISTORY > 5)
        {
            return 16;
        }

    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP);
    timeClient.begin();
    timeClient.update();

    long EPOCH_DATE = (timeClient.getEpochTime() - (86400 * USER_PARAM.OPEN_WEATHER_HISTORY));
    timeClient.end();

    sprintf(historyURL,"%s?lat=%.6f&lon=%.6f%s%ld&units=%s%s%s",TS_URL1,USER_PARAM.OPEN_WEATHER_LATITUDE,USER_PARAM.OPEN_WEATHER_LONGITUDE,TS_URL2,EPOCH_DATE,units,DS_URL3,USER_PARAM.OPEN_WEATHER_DKEY);




    HTTPClient http;
    http.begin(historyURL);
    int httpCode = http.GET();

    if (httpCode > 399)
        {
            if(httpCode == 401)
                {
                    http.end();
                    return 17;
                }
            else
                {
                    http.end();
                    return 23;
                }
        }


    DynamicJsonDocument doc(8192);

    deserializeJson(doc, http.getString());

    strncpy(location.timezone,doc["timezone"],50);
    location.timezoneOffset = doc["timezone_offset"];

    if(!history)
        {
            history = (struct HISTORICAL *)calloc(25,sizeof(struct HISTORICAL));
        }

    JsonObject current = doc["current"];
    history[0].dayTime = current["dt"]; // 1607292481
    history[0].sunrise = current["sunrise"]; // 1607256309
    history[0].sunset = current["sunset"]; // 1607290280
    history[0].temperature = current["temp"]; // 35.82
    history[0].apparentTemperature = current["feels_like"]; // 21.7
    history[0].pressure = current["pressure"]; // 1010
    history[0].humidity = current["humidity"]; // 51
    history[0].dewPoint = current["dew_point"]; // 20.88
    history[0].uvIndex = current["uvi"]; // 1.54
    history[0].cloudCover = current["clouds"]; // 1
    history[0].visibility = current["visibility"]; // 16093
    history[0].windSpeed = current["wind_speed"]; // 16.11
    history[0].windBearing = current["wind_deg"]; // 300
    history[0].windGust = current["wind_gust"]; // 24.16

    JsonObject current_weather_0 = current["weather"][0];
    history[0].id = current_weather_0["id"]; // 800
    history[0].main = (char *)realloc(history[0].main,sizeof(char) * strlen(current_weather_0["main"])+1);
    strncpy(history[0].main,current_weather_0["main"],strlen(current_weather_0["main"])+1);
    history[0].summary = (char *)realloc(history[0].summary,sizeof(char) * strlen(current_weather_0["description"])+1);
    strncpy(history[0].summary,current_weather_0["description"],strlen(current_weather_0["description"])+1);
    history[0].icon = (char *)realloc(history[0].icon,sizeof(char) * strlen(current_weather_0["icon"])+1);
    strncpy(history[0].icon,current_weather_0["icon"],strlen(current_weather_0["icon"])+1);

    OpenWeatherOneCall::setWeekdayName(history[0].dayTime, 0);

    JsonArray hourly = doc["hourly"];

    for(int x = 1; x < 24; x++)
        {
            JsonObject hourly_0 = hourly[x-1];
            history[x].dayTime = hourly_0["dt"]; // 1607212800
            history[x].sunrise = current["sunrise"] | 1607256309; // 1607256309
            history[x].sunset = current["sunset"] | 1607290280; // 1607290280
            history[x].temperature = hourly_0["temp"]; // 40.21
            history[x].apparentTemperature = hourly_0["feels_like"]; // 29.39
            history[x].pressure = hourly_0["pressure"]; // 1007
            history[x].humidity = hourly_0["humidity"]; // 56
            history[x].dewPoint = hourly_0["dew_point"]; // 26.51
            history[x].cloudCover = hourly_0["clouds"]; // 1
            history[x].visibility = hourly_0["visibility"]; // 16093
            history[x].windSpeed = hourly_0["wind_speed"]; // 11.41
            history[x].windBearing = hourly_0["wind_deg"]; // 290

            JsonObject hourly_0_weather_0 = hourly_0["weather"][0];
            history[x].id = hourly_0_weather_0["id"]; // 800
            history[x].main = (char *)realloc(history[x].main,sizeof(char) * strlen(hourly_0_weather_0["main"])+1);
            strncpy(history[x].main,hourly_0_weather_0["main"],strlen(hourly_0_weather_0["main"])+1);
            history[x].summary = (char *)realloc(history[x].summary,sizeof(char) * strlen(hourly_0_weather_0["description"])+1);
            strncpy(history[x].summary,hourly_0_weather_0["description"],strlen(hourly_0_weather_0["description"])+1);
            history[x].icon = (char *)realloc(history[x].icon,sizeof(char) * strlen(hourly_0_weather_0["icon"])+1);
            strncpy(history[x].icon,hourly_0_weather_0["icon"],strlen(hourly_0_weather_0["icon"])+1);

            OpenWeatherOneCall::setWeekdayName(history[0].dayTime, x);
        }

    http.end();
    return 0;
}

void OpenWeatherOneCall::setWeekdayName(long dayTime, int x)
{
    long DoW = dayTime / 86400L;
    int day_of_week = (DoW + 4) % 7;

    strncpy(history[x].weekDayName,short_names[day_of_week],4);

}

int OpenWeatherOneCall::createCurrent(int sizeCap)
{

    char getURL[200] = {0};

    sprintf(getURL,"%s?lat=%.6f&lon=%.6f%s%s%s%s",DS_URL1,USER_PARAM.OPEN_WEATHER_LATITUDE,USER_PARAM.OPEN_WEATHER_LONGITUDE,DS_URL2,units,DS_URL3,USER_PARAM.OPEN_WEATHER_DKEY);

    HTTPClient http;
    http.begin(getURL);
    int httpCode = http.GET();

    if (httpCode > 399)
        {
            if(httpCode == 401)
                {
                    http.end();
                    return 17;
                }

            http.end();
            return 23;

        }

    const size_t capacity = sizeCap;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, http.getString());
    doc.shrinkToFit();

    strncpy(location.timezone,doc["timezone"],50);
    location.timezoneOffset = doc["timezone_offset"];

//    memSize = ESP.getMaxAllocHeap();
//    printf("Line: %d memSize: %ld\n",__LINE__,memSize);

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
                            printf("calloc(nowData) failed!!\n\n");
                            return 0;
                        }
                }


            JsonObject currently = doc["current"];
            current->dayTime = currently["dt"]; // 1586781931
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
            current->id = currently["weather"][0]["id"];

            current->main = (char *)realloc(current->main,sizeof(char) * strlen(currently["weather"][0]["main"])+1);
            strncpy(current->main,currently["weather"][0]["main"],strlen(currently["weather"][0]["main"])+1);

            current->summary = (char *)realloc(current->summary,sizeof(char) * strlen(currently["weather"][0]["description"])+1);
            strncpy(current->summary,currently["weather"][0]["description"],strlen(currently["weather"][0]["description"])+1);

            current->icon = (char *)realloc(current->icon,sizeof(char) * strlen(currently["weather"][0]["icon"])+1);
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
                }

            JsonArray daily = doc["daily"];
            for (int x = 0; x < 8; x++)
                {
                    forecast[x].dayTime = daily[x]["dt"]; // 1586793600
                    forecast[x].sunriseTime = daily[x]["sunrise"]; // 1586773262
                    forecast[x].sunsetTime = daily[x]["sunset"]; // 1586820773

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
                    forecast[x].windBearing = daily[x]["wind_deg"]; // 180

                    forecast[x].id = daily[x]["weather"][0]["id"]; // 800

                    if(daily[x]["weather"][0]["main"])
                        {
                            forecast[x].main = (char *)realloc(forecast[x].main,sizeof(char) * strlen(daily[x]["weather"][0]["main"])+1);
                            strncpy(forecast[x].main,daily[x]["weather"][0]["main"],strlen(daily[x]["weather"][0]["main"])+1);
                        }

                    if(daily[x]["weather"][0]["description"])
                        {
                            forecast[x].summary = (char *)realloc(forecast[x].summary,sizeof(char) * strlen(daily[x]["weather"][0]["description"])+1);
                            strncpy(forecast[x].summary,daily[x]["weather"][0]["description"],strlen(daily[x]["weather"][0]["description"])+1);
                        }

                    if(daily[x]["weather"][0]["icon"])
                        {
                            forecast[x].icon = (char *)realloc(forecast[x].icon,sizeof(char) * strlen(daily[x]["weather"][0]["icon"])+1);
                            strncpy(forecast[x].icon,daily[x]["weather"][0]["icon"],strlen(daily[x]["weather"][0]["icon"])+1);
                        }

                    forecast[x].cloudCover = daily[x]["clouds"]; // 95
                    forecast[x].pop = daily[x]["pop"]; // 95
                    forecast[x].uvIndex = daily[x]["uvi"]; // 6.31

                    long DoW = forecast[x].dayTime / 86400L;
                    int day_of_week = (DoW + 4) % 7;

                    strncpy(forecast[x].weekDayName,short_names[day_of_week],4);
                }

        }


    if(exclude.alerts)
        {
            OpenWeatherOneCall::freeAlertMem();
        }
    else
        {
            if(!alert)
                {
                    alert = (struct ALERTS *)calloc(1,sizeof(struct ALERTS));
                }


            JsonObject ALERTS_0 = doc["alerts"][0];

            if(ALERTS_0["sender_name"])
                {
                    alert->senderName = (char *)realloc(alert->senderName,sizeof(char) * strlen(ALERTS_0["sender_name"])+1);
                    strncpy(alert->senderName,ALERTS_0["sender_name"],strlen(ALERTS_0["sender_name"])+1);
                }

            if(ALERTS_0["event"])
                {
                    alert->event = (char *)realloc(alert->event,sizeof(char) * strlen(ALERTS_0["event"])+1);
                    strncpy(alert->event,ALERTS_0["event"],strlen(ALERTS_0["event"])+1);
                }

            alert->alertStart = ALERTS_0["start"];
            alert->alertEnd = ALERTS_0["end"];

            if(ALERTS_0["description"])
                {
                    alert->summary = (char *)realloc(alert->summary,sizeof(char) * strlen(ALERTS_0["description"])+1);
                    strncpy(alert->summary,ALERTS_0["description"],strlen(ALERTS_0["description"])+1);
                }
        }


    if(exclude.hourly)
        {
            OpenWeatherOneCall::freeHourMem();
        }
    else
        {
            if(!hour)
                {
                    hour = (struct HOURLY *)calloc(48, sizeof(struct HOURLY));
                }

            JsonArray hourly = doc["hourly"];
            for(int h = 0; h < 48; h++)
                {

                    JsonObject hourly_0 = hourly[h];
                    hour[h].dayTime = hourly_0["dt"]; // 1604336400
                    hour[h].temperature = hourly_0["temp"]; // 46.58
                    hour[h].apparentTemperature = hourly_0["feels_like"]; // 28.54
                    hour[h].pressure = hourly_0["pressure"]; // 1015
                    hour[h].humidity = hourly_0["humidity"]; // 31
                    hour[h].dewPoint = hourly_0["dew_point"]; // 19.2
                    hour[h].cloudCover = hourly_0["clouds"]; // 20
                    hour[h].visibility = hourly_0["visibility"]; // 10000
                    hour[h].windSpeed = hourly_0["wind_speed"]; // 22.77
                    hour[h].windBearing = hourly_0["wind_deg"]; // 300

                    JsonObject hourly_0_weather_0 = hourly_0["weather"][0];
                    hour[h].id = hourly_0_weather_0["id"]; // 801

                    if(hourly_0_weather_0["main"])
                        {
                            hour[h].main = (char *)realloc(hour[h].main,sizeof(char) * strlen(hourly_0_weather_0["main"])+1);
                            strncpy(hour[h].main,hourly_0_weather_0["main"],strlen(hourly_0_weather_0["main"])+1);
                        }

                    if(hourly_0_weather_0["description"])
                        {
                            hour[h].summary = (char *)realloc(hour[h].summary,sizeof(char) * strlen(hourly_0_weather_0["description"])+1);
                            strncpy(hour[h].summary,hourly_0_weather_0["description"],strlen(hourly_0_weather_0["description"])+1);
                        }

                    if(hourly_0_weather_0["icon"])
                        {
                            hour[h].icon = (char *)realloc(hour[h].icon,sizeof(char) * strlen(hourly_0_weather_0["icon"])+1);
                            strncpy(hour[h].icon,hourly_0_weather_0["icon"],strlen(hourly_0_weather_0["icon"])+1);
                        }

                    hour[h].pop = hourly_0["pop"]; // 0
                }
        }


    if(exclude.minutely)
        {
            OpenWeatherOneCall::freeMinuteMem();
        }
    else
        {
            if(!minute)
                {
                    minute = (struct MINUTELY *)calloc(61, sizeof(struct MINUTELY));
                }


            JsonArray minutely = doc["minutely"];

            for(int x = 0; x<61; x++)
                {
                    minute[x].dayTime = minutely[x]["dt"];
                    minute[x].precipitation = minutely[x]["precipitation"]; // 0
                }
        }

    http.end();

    //  memSize = ESP.getMaxAllocHeap();
    //  printf("Line: %d memSize: %ld\n",__LINE__,memSize);
    //  printf("Size_Capacity: %d\n",sizeCap);

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

    strcat(DS_URL2,"&units=");

    return excludeMemSize;
}

int OpenWeatherOneCall::setOpenWeatherKey(char* owKey)
{
    int error_code = 0;
    if((strlen(owKey) < 25) || (strlen(owKey) > 64))
        {
            error_code += 12;
            strncpy(USER_PARAM.OPEN_WEATHER_DKEY,NULL,100);
            return error_code;
        };

    strncpy(USER_PARAM.OPEN_WEATHER_DKEY,owKey,100);

    return(EXIT_SUCCESS);

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
            break;
        case 2:
            strcpy(units,"IMPERIAL");
            break;
        case 1:
            strcpy(units,"METRIC");
            break;
        default :
            strcpy(units,"IMPERIAL");
        }

}

int OpenWeatherOneCall::setHistory(int _HIS)
{
    if(_HIS > 5)
        return 16;
    USER_PARAM.OPEN_WEATHER_HISTORY = _HIS;
}

// free routines

void OpenWeatherOneCall::freeCurrentMem(void)
{

    if(current)
        {
            free(current->icon);
            current->icon = NULL;
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
                    free(forecast[x-1].icon);
                    free(forecast[x-1].summary);
                    free(forecast[x-1].main);
                }
            free(forecast);
            forecast[0].icon = NULL;
            forecast[0].summary = NULL;
            forecast[0].main = NULL;
            forecast = NULL;
        }

}

void OpenWeatherOneCall::freeAlertMem(void)
{

    if(alert)
        {
            free(alert->senderName);
            free(alert->event);
            free(alert->summary);
            free(alert);
            alert->senderName = NULL;
            alert->event = NULL;
            alert->summary = NULL;
            alert = NULL;
        }
}

void OpenWeatherOneCall::freeHourMem(void)
{

    if(hour)
        {
            for( int x = 48; x > 0; x--)
                {
                    free(hour[x-1].icon);
                    free(hour[x-1].summary);
                    free(hour[x-1].main);
                }
            free(hour);
            hour[0].icon = NULL;
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

char* OpenWeatherOneCall::getErrorMsgs(int errorMsg){
   if(errorMsg > 25) errorMsg = 24;
strcpy_P(buffer, (char*)pgm_read_dword(&(errorMsgs[errorMsg])));
return buffer;
}


OpenWeatherOneCall::~OpenWeatherOneCall()
{

    OpenWeatherOneCall::freeCurrentMem();
    OpenWeatherOneCall::freeForecastMem();
    OpenWeatherOneCall::freeAlertMem();
    OpenWeatherOneCall::freeHourMem();
    OpenWeatherOneCall::freeMinuteMem();

}

// Looks like this is the end
