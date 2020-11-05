/*
   OpenWeatherOnecall.h
   Upgrade v2.0.0
   copyright 2020 - Jessica Hershey
   www.github.com/jHershey69

   7 day and current weather forecast based on current location
   NO GPS is required, uses WiFi triangulation to get location
*/

#ifndef DS
#define DS

#include "Arduino.h"


class OpenWeatherOneCall
{
public:
    //Constructor
    OpenWeatherOneCall();

    //Methods
    int parseWeather(char* DKEY, char* GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS, int CITY_ID, int API_EXCLUDES, int GET_HISTORY);


    //Variables

#define EXCL_C 1  //Exclude Current
#define EXCL_D 2  //Exclude Daily
#define EXCL_H 4  //Exclude Hourly
#define EXCL_M 8  //Exclude Minutely
#define EXCL_A 16 //Exclude Alerts


    //BITFIELDS for exclude flags

    typedef union FLAGS
    {
        struct
        {
            uint8_t current : 1;
            uint8_t daily : 1;
            uint8_t hourly : 1;
            uint8_t minutely : 1;
            uint8_t alerts : 1;
            uint8_t Spare2 : 1;
            uint8_t Spare1 : 1;
        };
        uint8_t all_excludes;
    };

    struct nowData
    {
        long dayTime; // 1582151288
        const char* summary; // "Clear"
        const char* icon; // "clear-day"
        int nearestStormDistance; // 50
        int nearestStormBearing; // 4
        int precipIntensity; // 0
        int precipProbability; // 0
        float temperature; // 46.38
        float apparentTemperature; // 41.49
        float dewPoint; // 17.18
        float humidity; // 0.31
        float pressure; // 1026.4
        float windSpeed; // 10.22
        float windGust; // 10.22
        int windBearing; // 348
        float cloudCover; // 0.02
        int uvIndex; // 0
        int visibility; // 10
        float ozone; // 323.4
    } current ;


    struct futureData
    {
        long dayTime; // 1582088400
        const char* summary; // "Partly cloudy throughout the day."
        const char* icon; // "partly-cloudy-day"
        long sunriseTime; // 1582112760
        long sunsetTime; // 1582151880
        float moonPhase; // 0.89
        float precipIntensity; // 0.0009
        float precipIntensityMax; // 0.0028
        long precipIntensityMaxTime; // 1582105560
        float precipProbability; // 0.3
        const char* precipType; // "rain"
        float temperatureHigh; // 51.24
        long temperatureHighTime; // 1582139280
        float temperatureLow; // 26.83
        long temperatureLowTime; // 1582199760
        float apparentTemperatureHigh; // 50.76
        long apparentTemperatureHighTime; // 1582139100
        float apparentTemperatureLow; // 19.9
        long apparentTemperatureLowTime; // 1582199940
        float dewPoint; // 26.79
        float humidity; // 0.54
        float pressure; // 1024.1
        float windSpeed; // 6.49
        float windGust; // 24.15
        long windGustTime; // 1582131600
        int windBearing; // 324
        float cloudCover; // 0.53
        int uvIndex; // 3
        long uvIndexTime; // 1582134180
        int visibility; // 10
        float ozone; // 322.6
        float temperatureMin; // 30.17
        long temperatureMinTime; // 1582174800
        float temperatureMax; // 51.24
        long temperatureMaxTime; // 1582139280
        float apparentTemperatureMin; // 24.39
        long apparentTemperatureMinTime; // 1582174800
        float apparentTemperatureMax; // 50.76
        long apparentTemperatureMaxTime; // 1582139100
    } forecast[8];


    struct HOURLY
    {
        long dt; // 1604336400
        float temp; // 46.58
        float feels_like; // 28.54
        int pressure; // 1015
        int humidity; // 31
        float dew_point; // 19.2
        int clouds; // 20
        int visibility; // 10000
        float wind_speed; // 22.77
        int wind_deg; // 300
        int id; // 801
        const char* main; // "Clouds"
        const char* description; // "few clouds"
        const char* icon; // "02d"
        int pop; // 0
    } hour[48] ;

    struct MINUTELY
    {
        long dt; // 1604341320
        int precipitation; // 0
    } minute[61];

    struct ALERTS
    {
        const char* sender_name; // "NWS Philadelphia - Mount Holly (New Jersey, Delaware, Southeastern Pennsylvania)"
        const char* event; // "Gale Watch"
        long start; // 1604271600
        long alert_end;
        const char* description;
    } alert[2];


    struct HISTORICAL
    {
        long dt; // 1604242490
        long sunrise; // 1604230151
        long sunset; // 1604267932
        float temp; // 285.9
        float feels_like; // 283.42
        int pressure; // 1016
        int humidity; // 76
        float dew_point; // 281.78
        float uvi; // 3.1
        int clouds; // 90
        int visibility; // 16093
        float wind_speed; // 3.1
        int wind_deg; // 160
        int id; // 804
        const char* main; // "Clouds"
        const char* description; // "overcast clouds"
        const char* icon; // "04d"

    }history[25];

    String short_names[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    float latitude;
    float longitude;


private:

    //Methods
    int getCoordinates(String googleKey);


    //Variables

    char units[10];

};

#endif
