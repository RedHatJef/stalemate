//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_PRINTSHT31INFO_H
#define STALEMATE_PRINTSHT31INFO_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class PrintSHT31Info : public MenuAction {
public:
    PrintSHT31Info() : MenuAction(F("Print SHT31 Info")) { }

    void doAction() override {
        Serial.println(F("SHT31 Temperature and Humidity Sensor: "));
        Serial.print(F("  Temp C     = "));
        Serial.println(getDevices()->sht->getTempC());
        Serial.print(F("  Temp F     = "));
        Serial.println(getDevices()->sht->getTempF());
        Serial.print(F("  Humidity   = "));
        Serial.print(getDevices()->sht->getHumidity());
        Serial.println('%');
    }
};

#endif //STALEMATE_PRINTSHT31INFO_H
