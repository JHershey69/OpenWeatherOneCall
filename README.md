# OpenWeatherOneCall v1.1.1
Open Weather One Call library to gather current and 7 day weather information
<br>Requires a One Call API Key from http://openweathermap.org<br>An optional GOOGLE API Key for geolocation through WiFi is available at https://developers.google.com/maps/documentation/javascript/get-api-key<br>Weather for ANY location can be gathered by sending a locations coordinates manually.
<hr>
This library is the easiest method for gathering information from One Weather using the One Call API.<br>For Dark Sky users migrating to a new system all previously used DarkSkySevenDay current and 7 day variables remain intact with noted exceptions<br>Future versions will incorporate hourly reports<br>OpenWeatherOneCall v1.1.1 includes a future implementation of CITY_ID as an option to geolocation or GPS coordinates. <strong>Please make sure you read all documentation to become familiar with the calling routine.</strong>If you are upgrading from a previous version it requires an additional parameter!<br>Thank you for your patience while we gather our wits<br>during this change in the weather.<br><hr><br>
# Variables <br>
<strong>**CURRENT WEATHER**</strong> <br>Access with OWOC.current.variable<hr>
temperature<br>apparentTemperature<br>pressure<br>humidity<br>dewPoint<br>uvIndex (midday number)<br>cloudCover<br>visibility<br>windSpeed<br>windBearing<br>windGust<br>icon<br><br>
<strong>**DEPRECATED DARKSKY VARIABLES**</strong><br>
nearestStormDistance<br>nearestStormBearing<br>precipIntensity<br>precipProbability<br>ozone<br><br><hr>
<strong>**FUTURE WEATHER**</strong> <br>Access with OWOC.forecast[x].variable<hr>
sunriseTime<br>sunsetTime<br>temperatureHigh<br>temperatureMin<br>temperaturemax<br>temperatureLow<br>apparentTemperatureHigh<br>apparentTemperatureLow<br>pressure<br>humidity<br>dewPoint<br>windSpeed<br>windBearing<br>precipType<br>icon<br>cloudCover<br>precipIntensity<br>uvIndex<br><br>
<strong>**DEPRECATED DARKSKY VARIABLES**</strong><br>
summary<br>moonPhase<br>precipIntensityMax<br>precipIntensityMaxTime<br>precipProbability<br>temperatureHighTime<br>temperatureLowTime<br>apparentTemperatureLowTime<br>windGust<br>windGustTime<br>uvIndexTime<br>ozone<br>temperatureMinTime<br>temperatureMaxTime<br>apparentTemperatureMaxTime<br>apparentTemperatureMinTime<br><br>
