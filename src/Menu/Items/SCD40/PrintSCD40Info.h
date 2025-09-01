//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_PRINTSCD40INFO_H
#define STALEMATE_PRINTSCD40INFO_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class PrintSCD40Info : public MenuAction {
public:
    PrintSCD40Info() : MenuAction(F("Print SCD40 Info")) { }

    void doAction() override {
        Serial.println(F("SCD40 CO2 Sensor: "));
        Serial.print(F("  Sensor Type  = "));
        Serial.println(getDevices()->scd40->getSensorType());
        Serial.print(F("  Serial Num   = "));
        Serial.println(getDevices()->scd40->getSerialNumber());
        Serial.print(F("  CO2 ppm      = "));
        Serial.println(getDevices()->scd40->getCO2PPM());
        Serial.print(F("  Temp C       = "));
        Serial.println(getDevices()->scd40->getTemperatureC());
        Serial.print(F("  Temp F       = "));
        Serial.println(getDevices()->scd40->getTemperatureF());
        Serial.print(F("  Humidity     = "));
        Serial.print(getDevices()->scd40->getHumidity());
        Serial.println('%');
        Serial.print(F("  TempOffset C = "));
        Serial.println(getDevices()->scd40->getTemperatureOffset());
        Serial.print(F("  Altitude M   = "));
        Serial.println(getDevices()->scd40->getSensorAltitude());
    }
};

#endif //STALEMATE_PRINTSCD40INFO_H
