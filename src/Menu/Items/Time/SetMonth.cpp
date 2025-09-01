//
// Created by redha on 9/1/2025.
//

#include "SetMonth.h"

void SetMonth::onValue(long value) {
    if(value > 12 || value < 1) {
        Serial.print(F("Month needs to be in the range (1..12)"));
        return;
    }

    getDevices()->clock->setMonth(value);
    printDateTime();
}