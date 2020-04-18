/*
   OpenWeatherOnecall.h
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
    void parseWeather(String DKEY, String GKEY, String UTYPE);


    //Variables
    struct nowData {
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


    struct futureData {
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

    String short_names[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    float latitude;
    float longitude;
    

  private:

    //Methods
    void getCoordinates(String googleKey);

    //Variables



};

#endif
