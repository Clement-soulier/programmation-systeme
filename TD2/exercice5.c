#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef enum{
    MON = 1, 
    TUE = 2, 
    WED = 3,
    THU = 4,
    FRI = 5,
    SAT = 6,
    SUN = 7
} dayname;

 typedef enum {
     JAN = 1, 
     FEB = 2,
     MAR = 3,
     APR = 4,
     MAY = 5,
     JUN = 6, 
     JUL = 7,
     AUG = 8, 
     SEP = 9, 
     OCT = 10,
     NOV = 11,
     DEC = 12
 } monthname;

 typedef struct {
     dayname weekday : 3;
     unsigned int day : 5;
     monthname month : 4;
     int year : 20;
 }date;

char* dayname_str(dayname day){
    switch (day) {
        case MON:
            return "Monday";
        case TUE:
            return "Tuesday";
        case WED:
            return "Wednessday";
        case THU:
            return "Thusday";
        case FRI:
            return "Friday";
        case SAT:
            return "Saturday";
        case SUN:
            return "Sunday";
        default:
            exit(-1);
    }
}


char* monthname_str(monthname month){
    switch (month) {
        case JAN:
            return "January";
        case FEB:
            return "February";
        case MAR:
            return "March";
        case APR:
            return "April";
        case MAY:
            return "May";
        case JUN:
            return "June";
        case JUL:
            return "July";
        case AUG:
            return "August";
        case SEP:
            return "September";
        case OCT:
            return "October";
        case NOV:
            return "November";
        case DEC:
            return "December";
        default:
            exit(-1);
    }
}

dayname weekday(time_t when){
    struct tm *time = localtime(&when);
    int day_number = time -> tm_wday;
    if(day_number == 0){
        return 7;
    }
    return day_number;
}

unsigned int get_monthday(time_t when){
    struct tm *time = localtime(&when);
    return time -> tm_mday;
}

monthname get_month(time_t when){
    struct tm *time = localtime(&when);
    return time -> tm_mon + 1;
}

int get_year(time_t when){
    struct tm *time = localtime(&when);
    return time -> tm_year + 1900;
}

int leapyear(unsigned int year){
    if(((year % 4 == 0) && !(year % 100 == 0)) || (year % 400 == 0)){
        return 1;
    }
    return 0;
}

date from_time(time_t when){
    date today;
    today.weekday = weekday(when);
    today.day = get_monthday(when);
    today.month = get_month(when);
    today.year = get_year(when);
    return today;
}

int main(void){
    date today = from_time(time(NULL));
    printf("Date: %s %i %s %i", dayname_str(today.weekday), today.day, monthname_str(today.month), today.year);
}
