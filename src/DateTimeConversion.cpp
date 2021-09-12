// OpenWeatherOneCall v3.1.7
// www.github.com/jhershey69

#include <time.h>
#include <stdio.h>
#include <string.h>

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
    */


    time_t rawtime = _epoch;
    struct tm *ptm = localtime(&rawtime);
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
        default:
            // M/D/Y 24H
            strftime(_buffer,20,"%m/%d/%Y %R",ptm);

        }

}
