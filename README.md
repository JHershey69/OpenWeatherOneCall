# OpenWeatherOneCall v4.0.3  ![IMAGE OF LIGHTNING](https://github.com/JHershey69/OpenWeatherOneCall/blob/master/images/lightning.jpg)
## This is for ESP32 only 

##This is an upgrade for OpenWeatherMap API 3.0 and Air Pollution API 2.5

**Please be advised** OpenWeather has changed ONE CALL to a subscription PAY BY CALL service. Signing up is
required to get your KEY, but you get 1000 calls a day for free (this is one call every 90 seconds and that is A LOT for free)
and you will continue to get all of the features of this library.

**OpenWeatherOneCall** library to gather weather information from OpenWeatherMap
<br>Requires a One Call API Key from http://openweathermap.org (Some fees involved for some actions, see documentation)<br>
<hr>
### Uses latitude and longitude from GPS, WIFI Triangulation, or other means user supplied ###
<br>
Please install these additional libraries (as needed) 

- [ESP32Time] for time functions
- [WiFiTri Library](https://www.github.com/jhershey69/WiFiTri) for WiFi Triangulation
-- A [GOOGLE API KEY](https://developers.google.com/maps/documentation/javascript/get-api-key) is required for WiFiTri
<hr>
The easiest method for gathering information from Open Weather Map using the ***One Call API***

For ***Dark Sky*** users migrating to a new system please refer to the Variables Document for changes.

*Please make sure you read the [**User Manual**](https://github.com/JHershey69/OpenWeatherOneCall/blob/master/docs/OpenWeatherOneCall%20Manual%20v3.0.4.pdf) as v3.0.3 had many changes and new information*

<br>**v4.0.0** 
<br>--MAJOR REWRITE with addition options and variable SEE DOCUMENTATION. Some features deprecated no longer supported.
<br>**v4.0.1** 
<br>--ADDED ISO 8601 Human Readable format to Date Time Format.
<br>**v4.0.3** 
<br>--Corrected a DATE numbering issue.


