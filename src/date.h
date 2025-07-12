#ifndef DATE_H
#define DATE_H

#include "lib/varvara.h"
#include "utils.h"

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} Date;

bool is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year) {
    if (month == 2) {
        return is_leap_year(year) ? 29 : 28;
    }
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month];
}

void init_date(Date* date, bool now) {
    if(now == FALSE) {
        date->year = 0;
        date->month = 0;
        date->day = 0;
        date->hour = 0;
        date->minute = 0;
        date->second = 0;
    } else {
        date->year = datetime_year();
        date->month = datetime_month();
        date->day = datetime_day();
        date->hour = datetime_hour();
        date->minute = datetime_minute();
        date->second = datetime_second();
    }
}

int total_minutes_date(Date* d) {
    int days = 0;
    int y, m;
    // Years
    for (y = 0; y < d->year - 2025; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }
    // Months
    for (m = 1; m < d->month; m++) {
        days += days_in_month(m, d->year);
    }
    // Days
    days += d->day - 1;
    // Total minutes
    return days * 24 * 60 + d->hour * 60 + d->minute;
}

int diff_min_date(Date* first, Date* last) {
    // Convert both dates to total minutes since year 0
    int total_minutes_first = 0;
    int total_minutes_last = 0;

    total_minutes_first = total_minutes_date(first);
    total_minutes_last = total_minutes_date(last);

    return total_minutes_last - total_minutes_first;
}

int compare_date(Date* a, Date* b) {
    // Compare years
    if (a->year < b->year) return -1;
    if (a->year > b->year) return 1;

    // Compare months
    if (a->month < b->month) return -1;
    if (a->month > b->month) return 1;

    // Compare days
    if (a->day < b->day) return -1;
    if (a->day > b->day) return 1;

    // Compare hours
    if (a->hour < b->hour) return -1;
    if (a->hour > b->hour) return 1;

    // Compare minutes
    if (a->minute < b->minute) return -1;
    if (a->minute > b->minute) return 1;

    // Compare seconds
    if (a->second < b->second) return -1;
    if (a->second > b->second) return 1;

    // If all components are equal
    return 0;
}

void copy_date(Date* src, Date* dest) {
    dest->second = src->second;
    dest->minute = src->minute;
    dest->hour = src->hour;
    dest->day = src->day;
    dest->month = src->month;
    dest->year = src->year;
}

void add_date(Date* now, Date* add) {
    // second management
    now->second += add->second % 60;
    now->minute += add->second / 60;

    // minute management 
    now->minute += add->minute;
    now->hour += now->minute / 60;
    now->minute = now->minute % 60;

    // hour management 
    now->hour += add->hour;
    now->day += now->hour / 24;
    now->hour = now->hour % 24;

    // day management 
    now->day += add->day;
    while (now->day > days_in_month(now->month, now->year)) {
        now->day -= days_in_month(now->month, now->year);
        now->month++;
        if (now->month > 12) {
            now->month = 1;
            now->year++;
        }
    }

    // month management
    now->month += add->month;
    while (now->month > 12) {
        now->month -= 12;
        now->year++;
    }

    // year management
    now->year += add->year;
}

void print_date(Date* date) {
    printInt(date->day);
    print("/");
    printInt(date->month + 1);
    print("/");
    printInt(date->year);
    print(" ");
    printInt(date->hour);
    print(":");
    printInt(date->minute);
    print("\n");
}

#endif 