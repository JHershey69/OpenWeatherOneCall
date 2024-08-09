/*
   Open Weather One Call Library
   v4.0.0
   Copyright 2020=2024 - Jessica Hershey
   www.github.com/JHershey69

   One Call API key at www.openweathermap.org
   Google Developer Key no longer required.

   Simple_Latitude_Longitude_Weather_Example.ino

   Returns ALL information and uses Latitude/Longitude, CITY ID, or IP Address
   If using a hotspot to connect your ESP32 to the WWW your results for IP
   search maybe be radically out of range. Please consult the documentation
   for use and variables available in the returned weather message

*/

// ===== Required libraries (Other required libraries are installed in header file)

#include <OpenWeatherOneCall.h>

// ======= END REQUIRED LIBRARIES =======================================



// ========= Required only for TFT Display ==============
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
//========= End TFT Library =============================


#define HOMESSID "YOUR SSID HERE"
#define HOMEPW "YOUR SSID PW HERE"
char ONECALLKEY[] = "YOUR ONE CALL KEY HERE";

// Only needed if WiFiTri installed for Triangulation may have fee from Google
// #define GOOGLEKEY "YOUR GOOGLE DEVELOPER API KEY""


//  OpenWeatherOneCall API Options and Settings

//*************** LOCATION ************************

int myTimeZone = -4;  //<--------- GMT OFFSET

//Location MODES are used to tell the program how you are delivering your coordinates:
// 1 LAT/LON  <----- Can be from GPS, manual as below, or whatever method gets you a latitude and longitude except IP location
// 2 CITYID   <----- From OpenWeatherMap City ID list. Not very accurate.
// 3 IP ADDRESS [NOT REALLY SUGGESTED CAN BE WILDLY OFFSET LOCATION IF HOTSPOT]

int locationMode = 1;

//For Latitude and Longitude Location MODE 1 setting if used
float myLATITUDE = 39.9537;  //<-----This is Toms River, NJ
float myLONGITUDE = -74.1979;

//For City ID Location MODE setting if used
int myCITYID = 4504476;  //<-----Toms River, NJ USA

//*************** END LOCATION **********************

//*************** FLAGS *****************************
int myCURRENT = 1;                      //<-----0 is CURRENT off, 1 is on
int myAIRQUALITY = 1;                   //<-----0 is Air Quality off, 1 is on
int myTIMESTAMP = 1;                    //<-----0 is TIMESTAMP off, 1 is TIMESTAMP on
char timestampDate[11] = "03/03/2023";  //<-----MM/DD/YYYY right here limit is 4 days in future, past is 01/01/1979
int myOVERVIEW = 1;                     //<-----0 is OVERVIEW off, 1 is OVERVIEW on
char overviewDate[9] = "TOMORROW";      //<-----"TODAY" or "TOMORROW"
//***************************************************

//************** EXCLUDES ***************************
// Excludes are EXCL_D, EXCL_H, EXCL_M, EXCL_A
// Those are DAILY, HOURLY, MINUTELY, and ALERTS
// You set them like this: myEXCLUDES = EXCL_D+EXCL_H+EXCL_M+EXCL_A
// If you leave them set to 0 as below you get a JSON file with ALL CURRENT WEATHER measurements which is a huge file

int myEXCLUDES = 0;  //<-----0 Excludes is default
//***************************************************

//*************** UNITS OF MEASURMENT and DATE TIME FORMAT ***************

int myUNITS = IMPERIAL;  //<-----METRIC, IMPERIAL, KELVIN (IMPERIAL is default)

//Date Time Format
int myDTF = 1;  //1 M/D/Y 24H
                // 2 D/M/Y 24H
                // 3 M/D/Y 12H
                // 4 D/M/Y 12H

//************************************************************************

//for debugging loop counting
int nextCall = 0;
//int callAttempt = 1;

OpenWeatherOneCall OWOC;  // <------ Invoke Library like this

// WiFi Connect function **********************************
void connectWifi() {
  WiFi.begin(HOMESSID, HOMEPW);
  printf("Connecting.");
  int TryNum = 0;
  while (WiFi.status() != WL_CONNECTED) {
    printf(".");
    delay(200);
    TryNum++;
    if (TryNum > 20) {
      printf("\nUnable to connect to WiFi. Please check your parameters.\n");
      for (;;)
        ;
    }
  }
  printf("Connected to: % s\n\n", HOMESSID);
}  //================== END WIFI CONNECT =======================



void setup() {
  // In this sketch for display of values
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("Serial Monitor Initialized");

  // WiFi Connection required *********************
  while (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }  //<----------------End WiFi Connection Check


  //Now call the weather. Please notice no arguments are required in this call
  OWOC.parseWeather();

  //Now display some information, note the pointer requirement for current and alert

  // Location info is available for ALL modes
  printf("\nLocation: % s, % s % s\n", OWOC.location.CITY, OWOC.location.STATE, OWOC.location.COUNTRY);


  //Verify all other values exist before using

  if (OWOC.current)  //Check if data is in the struct for all variables before using them
  {
    printf("\nCURRENT\n");
    printf("Temp : % .0f\n", OWOC.current->temperature);
    printf("Humidity : % .0f\n", OWOC.current->humidity);
  } else printf("\nCURRENT IS OFF or EMPTY\n");

  //Forecast is part of CURRENT and has info for the 8 days future weather. See variables PDF
  // The JSON holds this info in the DAILY section and EXCL_D turns this off
  if (OWOC.forecast) {  //Check if data is in the struct for all variables before using them
    printf("\nFORECAST - Up to 8 days future forecast\n");
    for (int x = 0; x < 8; x++) {
      printf("Date: %s High Temperature: % .0f\n", OWOC.forecast[x].readableDateTime, OWOC.forecast[x].temperatureHigh);
    }
  } else printf("\nFORECAST IS OFF or EMPTY\n");

  //Hour is part of CURRENT and has info for 48 hours future weather. See variables PDF
  // The JSON holds this info in the HOURLY section and EXCL_H turns this off
  if (OWOC.hour) {  //Check if data is in the struct for all variables before using them
    printf("\nHOURLY   - Up to 48 hours forecast\n");

    for (int x = 0; x < 6; x++) {
      printf("%s Actual Temp: % .0f\n", OWOC.hour[x].readableTime, OWOC.hour[x].temperature);
      printf("%s Feels Like Temp: % .0f\n", OWOC.hour[x].readableTime, OWOC.hour[x].apparentTemperature);
    }
  } else printf("\nHOURLY IS OFF or EMPTY\n");

   //MINUTELY is part of CURRENT and has info for 60 minutes future weather. See variables PDF
   // The JSON holds this info in the MINUTELY section and EXCL_M turns this off
  if (OWOC.minute) {  //Check if data is in the struct for all variables before using them
    printf("\nMINUTELY   - Up to 60 minutes precipitation forecast\n");

    for (int x = 0; x < 30; x++) {
      printf("%s Precipitation: % .2f\n", OWOC.minute[x].readableTime, OWOC.minute[x].precipitation);
    }
  } else printf("\nHOURLY IS OFF or EMPTY\n");

   //ALERTS is part of CURRENT and has info for alerts in your area. See variables PDF
   // The JSON holds this info in the ALERTS section and EXCL_A turns this off
  if (OWOC.alert)  //Check if data is in the struct for all variables before using them
  {
    printf("\nALERT *** ALERT *** ALERT\n");
    printf("Sender : % s\n", OWOC.alert->senderName);
    printf("Event : % s\n", OWOC.alert->event);
    printf("ALERT : % s\n", OWOC.alert->summary);
  } else {
    printf("\nNo Alerts For Area, Alerts are EXCLUDED, or Alerts struct is empty\n");
  }

   //The following sections are separate from CURRENT and are turned off and on with their own flags

  if (OWOC.timestamp) {  //Check if data is in the struct for all variables before using them
    printf("\nTIMESTAMP - Single day weather for any date from 01-01-1979 to 4 days in future\n");
    printf("% s - % s\nTemperature: % .0f\nFeels like: % .0f\nHumidity: % .0F%%\n", OWOC.timestamp[0].weekDayName, OWOC.timestamp[0].readableDateTime, OWOC.timestamp[0].temperature,OWOC.timestamp[0].apparentTemperature,OWOC.timestamp[0].humidity);
  } else printf("\nTIMESTAMP IS OFF or EMPTY\n");

  if (OWOC.overView)  //Check if data is in the struct for all variables before using them
  {
    printf("\nOVERVIEW - Summary for TODAY or TOMORROW generated by AI\n");

    // Print the struct values to verify
    printf("Latitude: %.6f\n", OWOC.overView->lat);  // %.6f for printing double with 6 decimal places
    printf("Longitude: %.6f\n", OWOC.overView->lon);
    printf("Timezone: %s\n", OWOC.overView->tz);  // %s for const char* (string)
    printf("Date: %s\n", OWOC.overView->date);
    printf("Units: %s\n", OWOC.overView->units);
    printf("Weather Overview: %s\n", OWOC.overView->weather_overview);

  } else printf("\nOVERVIEW IS OFF or EMPTY\n");

  if (OWOC.quality)  //Check if data is in the struct for all variables before using them
  {
    printf("\nAir Quality and Pollution Numbers for today");
    printf("\nAir Quality : % d\n", OWOC.quality->aqi);
    printf("Carbon : % .2f\n", OWOC.quality->co);
    printf("Nitrogen : % .2f\n", OWOC.quality->no);
    printf("Ozone : % .2f\n", OWOC.quality->o3);
  } else printf("\nAIR QUALITY IS OFF or EMPTY\n");

 
  
}

void loop() {
}
