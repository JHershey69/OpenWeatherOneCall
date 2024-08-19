// OpenWeatherOneCall v4.0.0
// www.github.com/jhershey69
// DO NOT EDIT

#include "OpenWeatherOneCall.h"


// Takes any EPOCH time converts to Human Readable
void dateTimeConversion(long _epoch, char *_buffer, int _format)
{

    /*FORMAT RETURNS
    1 M/D/Y 24H
    2 D/M/Y 24H
    3 M/D/Y 12H
    4 D/M/Y 12H
    5/6 TIME ONLY 24H
    7/8 TIME ONLY 12H
    9 DAY SHORTNAME
    10 M/D/Y ONLY
    11 D/M/Y ONLY

    ISO8601 options
    12 YYYY-MM-DD ONLY
    13 THH:MM:SS ONLY
    14 YYYY-MM-DDTHH:MM:SS
    */

    // NTP Server
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = myTimeZone * 3600;
    const int   daylightOffset_sec = 0;

// Initialize NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);



    time_t rawtime = _epoch;
    struct tm *ptm = localtime(&rawtime);

    // Add 1 to the day
    ptm->tm_mday += 1;

    // Normalize the time structure (this handles overflow of days, months, etc.)
    mktime(ptm);


    switch (_format)
        {

        case 1:
            // M/D/Y 24H
            strftime(_buffer,20,"%m/%d/%Y %R",ptm);
            break;
        case 2:
            // D/M/Y 24H
            strftime(_buffer,20,"%d/%m/%Y %R",ptm);
            break;
        case 3:
            // M/D/Y 12H
            strftime(_buffer,20,"%m/%d/%Y %r",ptm);
            break;
        case 4:
            // D/M/Y 12H
            strftime(_buffer,20,"%d/%m/%Y %r",ptm);
            break;
        case 5:
        case 6:
            // 24 HOUR TIME ONLY
            strftime(_buffer,20,"%R",ptm);
            break;
        case 7:
        case 8:
            // 12 HOUR TIME ONLY
            strftime(_buffer,20,"%r",ptm);
            break;
        case 9:
            // DAY SHORTNAME
            strftime(_buffer,20,"%a",ptm);
            break;
        case 10:
            // M/D/Y 24H
            strftime(_buffer,20,"%m/%d/%Y",ptm);
            break;
        case 11:
            // D/M/Y 24H
            strftime(_buffer,20,"%d/%m/%Y",ptm);
            break;
        case 12:
            // ISO 8601 YYYY-MM-DD
            strftime(_buffer,20,"%F",ptm);
            break;
        case 13:
            // ISO8601 THH:MM:SS
            strftime(_buffer,20,"T%T",ptm);
            break;
        case 14:
            // ISO8601 YYYY-MM-DDTHH:MM:SS
            strftime(_buffer,20,"%FT%T",ptm);
            break;
        default:
            // M/D/Y 24H
            strftime(_buffer,20,"%m/%d/%Y",ptm);

        }

}




