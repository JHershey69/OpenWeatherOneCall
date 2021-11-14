# OpenWeatherOneCall v3.1.9   ![IMAGE OF LIGHTNING](https://github.com/JHershey69/OpenWeatherOneCall/blob/master/images/lightning.jpg)
## This is for ESP32 only 

**OpenWeatherOneCall** library to gather weather information from OpenWeatherMap
<br>Requires a One Call API Key from http://openweathermap.org (Some fees involved for some actions, see documentation)<br>
<hr>
### Uses latitude and longitude from GPS, WIFI Triangulation, or other means user supplied ###
<br>
Please install these additional libraries (as needed) 

- [WiFiTri Library](https://www.github.com/jhershey69/WiFiTri) for WiFi Triangulation
-- A [GOOGLE API KEY](https://developers.google.com/maps/documentation/javascript/get-api-key) is required for WiFiTri
<hr>
The easiest method for gathering information from Open Weather Map using the ***One Call API***

For ***Dark Sky*** users migrating to a new system please refer to the Variables Document for changes.

*Please make sure you read the [**User Manual**](https://github.com/JHershey69/OpenWeatherOneCall/blob/master/docs/OpenWeatherOneCall%20Manual%20v3.0.4.pdf) as v3.0.3 had many changes and new information*

v3.0.0 has a **Legacy Mode** to maintain ease of use for previous versions
<br>**v3.0.2** 
<br>--fixed a couple of things so I could release the same thng with a new number
<br>**v3.0.3** 
<br>--added HUMAN READABLE date and time formats, see manual and variable sheet.
<br>--Date formats now selectable MM/DD/YY, DD/MM/YY. Time available in 24 or 12 Hour format.
<br>--Remove dependancies on all external libraries. Only ArduinoJson v6+ is required now.
<br>**v3.0.4** 
<br>--added HUMAN READABLE sunrise and sunset for CURRENT, see manual and variable sheet.
<br>--added snowVolume and rainVolume, see variables sheet, returned in mm or in based on units
<br>**v3.0.5** 
<br>--minor bug fix with language selection.
<br>**v3.1.0** 
<br>--added Air Quality information (see addendum).
<br>**v3.1.1** 
<br>--minor bug fix to historical rain and snow.
<br>**v3.1.2** 
<br>--minor bug fix to library files.
<br>**v3.1.3, v3.1.4** 
<br>--minor bug fix to historical rain and snow.
<br>**v3.1.5** 
<br>--Multiple Alerts update (see added docs).
<br>**v3.1.6** 
<br>--Multiple Alerts memory fix.
<br>**v3.1.7, 3.1.8** 
<br>--Fixed missing code.
<br>**v3.1.9** 
<br>--Minor fix to clear old alerts.
