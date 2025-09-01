//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_CLOCK_H
#define ETHAN1_CLOCK_H


#include <stdint.h>

class Clock {
public:
    Clock();
    void init();
    void update();

    [[nodiscard]] const char* getTimeString() const;
    [[nodiscard]] const char* getDateString() const;

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
};


#endif //ETHAN1_CLOCK_H
