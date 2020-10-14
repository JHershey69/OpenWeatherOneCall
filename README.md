# OpenWeatherOneCall v1.2.2
(THIS IS FOR THE ESP32, does NOT operate on other Arduino systems)

Open Weather One Call library to gather current and 7 day weather information
<br>Requires a One Call API Key from http://openweathermap.org<br>An <strong>optional</strong> GOOGLE API Key for geolocation through WiFi is available at https://developers.google.com/maps/documentation/javascript/get-api-key<br>Weather for ANY location can be gathered by sending location coordinates manually, via GPS, or using a CITY ID.
<hr>
This library is the easiest method for gathering information from <strong>One Weather</strong> using the <strong>One Call API</strong>.<br><br><cite>For Dark Sky users migrating to a new system all previously used DarkSkySevenDay current and 7 day variables remain intact with noted exceptions</cite><br><br>Future versions will incorporate hourly reports<br><br><strong>OpenWeatherOneCall v1.2.0</strong> includes the implementation of CITY_ID as an option to geolocation or GPS coordinates. <strong><cite>Please make sure you read all documentation to become familiar with the calling routine.</cite></strong>If you are upgrading from a previous version it requires an additional parameter!<br>Thank you for your patience while we gather our wits<br>during this change in the weather.<br><hr><br>
# Variables <br>
<strong>**CURRENT WEATHER**</strong> <br>Access with OWOC.current.variable<hr>
temperature<br>apparentTemperature<br>pressure<br>humidity<br>dewPoint<br>uvIndex (midday number)<br>cloudCover<br>visibility<br>windSpeed<br>windBearing<br>windGust<br>icon<br><br>
<strong>**DEPRECATED DARKSKY VARIABLES**</strong><br>
nearestStormDistance<br>nearestStormBearing<br>precipIntensity<br>precipProbability<br>ozone<br><br><hr>
<strong>**FUTURE WEATHER**</strong> <br>Access with OWOC.forecast[x].variable<hr>
sunriseTime<br>sunsetTime<br>temperatureHigh<br>temperatureMin<br>temperaturemax<br>temperatureLow<br>apparentTemperatureHigh<br>apparentTemperatureLow<br>pressure<br>humidity<br>dewPoint<br>windSpeed<br>windBearing<br>precipType<br>icon<br>cloudCover<br>precipIntensity<br>uvIndex<br><br>
<strong>**DEPRECATED DARKSKY VARIABLES**</strong><br>
summary<br>moonPhase<br>precipIntensityMax<br>precipIntensityMaxTime<br>precipProbability<br>temperatureHighTime<br>temperatureLowTime<br>apparentTemperatureLowTime<br>windGust<br>windGustTime<br>uvIndexTime<br>ozone<br>temperatureMinTime<br>temperatureMaxTime<br>apparentTemperatureMaxTime<br>apparentTemperatureMinTime<br><br>
