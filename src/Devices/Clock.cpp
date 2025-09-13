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
        Serial.println(F("Failed to start RTC"));
        while(1) delay(10);
    }
}

void Clock::update() {
    DateTime now = rtc.now();
    clearStrings();
    snprintf(timeString, sizeof(timeString), "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
    snprintf(dateString, sizeof(dateString), "%02u/%02u/%04u", now.month(), now.day(), now.year());
}

void Clock::setYear(uint16_t val) {
    DateTime d = rtc.now();
    DateTime* newTime = new DateTime(val, d.month(), d.day(), d.hour(), d.minute(), d.second());
    rtc.adjust(*newTime);
}

void Clock::setMonth(uint8_t val) {
    DateTime d = rtc.now();
    DateTime* newTime = new DateTime(d.year(), val, d.day(), d.hour(), d.minute(), d.second());
    rtc.adjust(*newTime);
}

void Clock::setDay(uint8_t val) {
    DateTime d = rtc.now();
    DateTime* newTime = new DateTime(d.year(), d.month(), val, d.hour(), d.minute(), d.second());
    rtc.adjust(*newTime);
}

void Clock::setHours(uint8_t val) {
    DateTime d = rtc.now();
    DateTime* newTime = new DateTime(d.year(), d.month(), d.day(), val, d.minute(), d.second());
    rtc.adjust(*newTime);
}

void Clock::setMinutes(uint8_t val) {
    DateTime d = rtc.now();
    DateTime* newTime = new DateTime(d.year(), d.month(), d.day(), d.hour(), val, d.second());
    rtc.adjust(*newTime);
}

void Clock::setSeconds(uint8_t val) {
    DateTime d = rtc.now();
    DateTime* newTime = new DateTime(d.year(), d.month(), d.day(), d.hour(), d.minute(), val);
    rtc.adjust(*newTime);
}
