//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_SETPRESSURECOMPENSATION_H
#define STALEMATE_SETPRESSURECOMPENSATION_H

#include "Menu/MenuAction.h"

class SetPressureCompensation : public MenuAction {
public:
    SetPressureCompensation() : MenuAction(F("Set Pressure Compensation From BMP390")) { }

    void doAction() override {
        int altitudeM = round(getDevices()->bmp->getAltitudeM());

        if(altitudeM < 0 || altitudeM > 3000) {
            Serial.println(F("Altitude out of range, not setting pressure compensation."));
            return;
        }

        Serial.print(F("Setting altitude on ExplorIR to BMP reported meters: "));
        Serial.println(altitudeM);

        Serial.print(F("Using compensation calculation will result in setting: "));
        Serial.println(getDevices()->explorIR->getCompensationFromAltitudeM(altitudeM));

        getDevices()->explorIR->setAltitudeM(altitudeM);

        Serial.println(F("Re-fetching pressure compensation value."));
        getDevices()->explorIR->fetchPressureCompensation();
    }

private:
};

#endif //STALEMATE_SETPRESSURECOMPENSATION_H
