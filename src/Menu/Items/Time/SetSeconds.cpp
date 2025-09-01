//
// Created by redha on 9/1/2025.
//

#include "SetSeconds.h"

void SetSeconds::onValue(long value) {
    if(value > 59) {
        Serial.print(F("Seconds need to be in the range (0..59)"));
        return;
    }

    getDevices()->clock->setSeconds(value);
    printDateTime();
}