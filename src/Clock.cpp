//
// Created by redha on 3/10/2025.
//

#include "Clock.h"
#include "RTClib.h"

RTC_DS3231 rtc;

static const char daysOfTheWeek[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

Clock::Clock() {
    clearStrings();
}

const char *Clock::getTimeString() const {
    return timeString;
}

const char *Clock::getDateString() const {
    return dateString;
}

void Clock::clearStrings() {
    memset(timeString, 0, sizeof(timeString));
    memset(dateString, 0, sizeof(dateString));
}

void Clock::init() {
    if(!rtc.begin())
    {
        Serial.println("Failed to start RTC");
        while(1) delay(10);
    }
}

void Clock::update() {
    DateTime now = rtc.now();
    clearStrings();
    snprintf(timeString, sizeof(timeString), "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
    snprintf(dateString, sizeof(dateString), "%02u/%02u/%04u", now.month(), now.day(), now.year());
}
