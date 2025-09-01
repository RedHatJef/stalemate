//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_PRINTTIME_H
#define ETHAN1_PRINTTIME_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class PrintTime : public MenuAction {
public:
    PrintTime() : MenuAction("Print Time") {}
    void doAction() override {
        // Placeholder: on bare Arduino, we’ll show millis().
        // If you have RTC set up, you could print the real time here.
        Serial.print(F("Current time (ms since boot): "));
        Serial.println(millis());
    }
};


#endif //ETHAN1_PRINTTIME_H
