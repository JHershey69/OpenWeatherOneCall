#ifndef ERRMSGS_H_INCLUDED
#define ERRMSGS_H_INCLUDED


const char string_0[] PROGMEM = "Latitude out of bounds";
const char string_1[] PROGMEM = "Longitude out of bounds";
const char string_2[] PROGMEM = "Latitude and Longitude out of bounds";
const char string_3[] PROGMEM = "404 Unknown City ID";
const char string_4[] PROGMEM = "Malformed API City ID";
const char string_5[] PROGMEM = "Bad returned json latitude";
const char string_6[] PROGMEM = "Bad returned json longitude";
const char string_7[] PROGMEM = "404 IPIFY error";
const char string_8[] PROGMEM = "IPIFY unknown http";
const char string_9[] PROGMEM = "404 IPAPI error";
const char string_10[] PROGMEM = "IPAPI unknown http";
const char string_11[] PROGMEM = "Malformed ONECALLKEY";
const char string_12[] PROGMEM = "Bad JSON latitude and longitude";
const char string_13[] PROGMEM = "Excludes out of bounds";
const char string_14[] PROGMEM = "Invalid units type";
const char string_15[] PROGMEM = "History out of bounds";
const char string_16[] PROGMEM = "Malformed API Big Cloud";
const char string_17[] PROGMEM = "404 Big Cloud";
const char string_18[] PROGMEM = "Unknown http err Big Cloud";
const char string_19[] PROGMEM = "Locality JSON error";
const char string_20[] PROGMEM = "HTTP Err OpenWeatherMap";
const char string_21[] PROGMEM = "Malformed API";
const char string_22[] PROGMEM = "MALLOC FAIL";
const char string_23[] PROGMEM = "LAT/LON NOT SET";

const char *const errorMsgs[] PROGMEM =
{
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  string_7,
  string_8,
  string_9,
  string_10,
  string_11,
  string_12,
  string_13,
  string_14,
  string_15,
  string_16,
  string_17,
  string_18,
  string_19,
  string_20,
  string_21,
  string_22,
  string_23
};


#endif // ERRMSGS_H_INCLUDED
