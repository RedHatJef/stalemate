//
// Created by redha on 9/1/2025.
//


#include "PrintDateTime.h"

void PrintDateTime::doAction() {
    Serial.print(F("Current date/time: "));
    Serial.print(getDevices()->clock->getDateString());
    Serial.print("  ");
    Serial.println(getDevices()->clock->getTimeString());
}
