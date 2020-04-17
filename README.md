# OpenWeatherOneCall
Open Weather One Call library to gather current, hourly (48), and 7 day weather for your current location
<br>Requires a One Call API Key from http://openweathermap.org<br>Requires a GOOGLE API Key from https://developers.google.com/maps/documentation/javascript/get-api-key<br>Use the Open Weather Key number in place of any previously used  DARKSKYKEY number **(please see the example sketch)**
<hr>
This library is a direct migration from DarkSkySevenDay and can be used as a replacement INCLUDE for any program in which you used my DarkSkySevenDay (Change #include DarkSkySevenDay.h to #include OpenWeatherOneCall.h, then change the invoke line)<br>This can also be used in new projects on it's own.<br>All previously used DarkSkySevenDay current and 7 day variables remain intact with noted exceptions<br>Future versions will incorporate hourly reports<br><br>Thank you for your patience while we gather our wits<br>during this change in the weather.<br><hr><br>
# Variables <br>
**CURRENT WEATHER** <hr>
temperature<br>apparentTemperature<br>pressure<br>humidity<br>dewPoint<br>uvIndex (midday number)<br>cloudCover<br>visibility<br>windSpeed<br>windBearing<br>windGust<br>icon<br><br>
**DEPRECATED DARKSKY VARIABLES**<br>
nearestStormDistance<br>nearestStormBearing<br>precipIntensity<br>precipProbability<br>ozone<br><br><hr>
**FUTURE WEATHER** <hr>
sunriseTime<br>sunsetTime<br>temperatureHigh<br>temperatureMin<br>temperaturemax<br>temperatureLow<br>apparentTemperatureHigh<br>apparentTemperatureLow<br>pressure<br>humidity<br>dewPoint<br>windSpeed<br>windBearing<br>precipType<br>icon<br>cloudCover<br>precipIntensity<br>uvIndex<br><br>
**DEPRECATED DARKSKY VARIABLES**<br>
summary<br>moonPhase<br>precipIntensityMax<br>precipIntensityMaxTime<br>precipProbability<br>temperatureHighTime<br>temperatureLowTime<br>apparentTemperatureLowTime<br>windGust<br>windGustTime<br>uvIndexTime<br>ozone<br>temperatureMinTime<br>temperatureMaxTime<br>apparentTemperatureMaxTime<br>apparentTemperatureMinTime<br><br>
