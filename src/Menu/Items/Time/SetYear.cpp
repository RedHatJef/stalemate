//
// Created by redha on 9/1/2025.
//

#include "SetYear.h"

void SetYear::onValue(long value) {
    if(value < 2025 || value > 2099) {
        Serial.print(F("Year needs to be in the range (2025..2099)"));
        return;
    }

    getDevices()->clock->setYear(value);
    printDateTime();
}