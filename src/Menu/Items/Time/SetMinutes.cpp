//
// Created by redha on 9/1/2025.
//

#include "SetMinutes.h"

void SetMinutes::onValue(long value) {
    if(value > 59) {
        Serial.print(F("Minute needs to be in the range (0..59)"));
        return;
    }

    getDevices()->clock->setMinutes(value);
    printDateTime();
}
