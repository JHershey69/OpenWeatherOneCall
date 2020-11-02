# OpenWeatherOneCall v1.3.0
(THIS IS FOR THE ESP32, does NOT operate on other Arduino systems)

Open Weather One Call library to gather current and 7 day weather information
<br>Requires a One Call API Key from http://openweathermap.org<br>An <strong>optional</strong> GOOGLE API Key for geolocation through WiFi is available at https://developers.google.com/maps/documentation/javascript/get-api-key<br>Weather for ANY location can be gathered by sending location coordinates manually, via GPS, or using a CITY ID.
<hr>
This library is the easiest method for gathering information from <strong>One Weather</strong> using the <strong>One Call API</strong>.<br><br><cite>For Dark Sky users migrating to a new system all previously used DarkSkySevenDay current and 7 day variables remain intact with noted exceptions</cite><br><br>Future versions will incorporate hourly reports<br><br><strong>OpenWeatherOneCall v1.2.0</strong> included the implementation of CITY_ID as an option to geolocation or GPS coordinates, <strong>OpenWeatherOneCall v1.3.0</strong> brings you the option to designate EXCLUDEs in your call to avoid receiving info and using precious memory space. <strong><cite>Please make sure you read all documentation to become familiar with the calling routine.</cite></strong>If you are upgrading from a previous version it requires an additional parameter!<br>Thank you for your patience while we gather our wits<br>during this change in the weather.<br><hr><br>

<br>If you are coming here from the DarkSky service, please be sure you understand which variables no longer exist!<br>
<strong>**DEPRECATED DARKSKY VARIABLES FOR CURRENT WEATHER**</strong><br>
nearestStormDistance<br>nearestStormBearing<br>precipIntensity<br>precipProbability<br>ozone<br><br><hr>
<strong>**DEPRECATED DARKSKY VARIABLES FOR FUTURE FORECASTS**</strong><br>
summary<br>moonPhase<br>precipIntensityMax<br>precipIntensityMaxTime<br>precipProbability<br>temperatureHighTime<br>temperatureLowTime<br>apparentTemperatureLowTime<br>windGust<br>windGustTime<br>uvIndexTime<br>ozone<br>temperatureMinTime<br>temperatureMaxTime<br>apparentTemperatureMaxTime<br>apparentTemperatureMinTime<br><br>
<strong>PLEASE, PLEASE, PLEASE read the supplied documentation!</strong><br><br>
