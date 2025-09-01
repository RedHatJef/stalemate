//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_CLOCK_H
#define ETHAN1_CLOCK_H


class Clock {
public:
    Clock();
    void init();
    void update();

    const char* getTimeString() const;
    const char* getDateString() const;

private:
    void clearStrings();
    char timeString[16];
    char dateString[16];
};


#endif //ETHAN1_CLOCK_H
