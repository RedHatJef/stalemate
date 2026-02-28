//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_CLOCK_H
#define STALEMATE_CLOCK_H

#include "RTClib.h"

class Clock {
public:
    Clock();
    void init();
    void update();

    [[nodiscard]] const char* getTimeString() const;
    [[nodiscard]] const char* getDateString() const;

    [[nodiscard]] uint16_t getYear()    const { return now.year(); }
    [[nodiscard]] uint8_t  getMonth()   const { return now.month(); }
    [[nodiscard]] uint8_t  getDay()     const { return now.day(); }
    [[nodiscard]] uint8_t  getHour()    const { return now.hour(); }
    [[nodiscard]] uint8_t  getMinute()  const { return now.minute(); }
    [[nodiscard]] uint8_t  getSecond()  const { return now.second(); }

    [[nodiscard]] uint32_t getEpochSeconds() const { return now.unixtime(); }

    void setYear(uint16_t val);
    void setMonth(uint8_t val);
    void setDay(uint8_t val);
    void setHours(uint8_t val);
    void setMinutes(uint8_t val);
    void setSeconds(uint8_t val);

private:
    void clearStrings();
    char timeString[16];
    char dateString[16];
    DateTime now;
};


#endif //STALEMATE_CLOCK_H
