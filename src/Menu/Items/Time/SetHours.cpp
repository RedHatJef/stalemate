//
// Created by redha on 9/1/2025.
//

#include "SetHours.h"

void SetHours::onValue(long value) {
    if(value > 23) {
        Serial.print(F("Hour needs to be in the range (0..23)"));
        return;
    }

    getDevices()->clock->setHours(value);
    printDateTime();
}
