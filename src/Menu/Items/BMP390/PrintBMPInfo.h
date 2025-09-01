//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_PRINTBMPINFO_H
#define STALEMATE_PRINTBMPINFO_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class PrintBMPInfo : public MenuAction {
public:
    PrintBMPInfo() : MenuAction(F("Print BMP390 Info")) { }

    void doAction() override {
        Serial.print(F("BMP390: ChipId = "));
        Serial.println(getDevices()->bmp->getChipID());
        Serial.print(F("  Temp C     = "));
        Serial.println(getDevices()->bmp->getTempC());
        Serial.print(F("  Temp F     = "));
        Serial.println(getDevices()->bmp->getTempF());
        Serial.print(F("  Altitude M = "));
        Serial.println(getDevices()->bmp->getAltitudeM());
        Serial.print(F("  Altitude F = "));
        Serial.println(getDevices()->bmp->getAltitudeF());
    }
};

#endif //STALEMATE_PRINTBMPINFO_H
