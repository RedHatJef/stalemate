//
// Created by redha on 9/1/2025.
//

#include "SetDay.h"

void SetDay::onValue(long value) {
    if(value > 31 || value < 1) {
        Serial.print(F("Day of Month needs to be in the range (1..31)"));
        return;
    }

    getDevices()->clock->setDay(value);
    printDateTime();
}