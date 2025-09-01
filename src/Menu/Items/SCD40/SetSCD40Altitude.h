//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETSCD40ALTITUDE_H
#define STALEMATE_SETSCD40ALTITUDE_H

#include "SetSCD40Parameter.h"

class SetSCD40Altitude : public SetSCD40Parameter {
public:
    SetSCD40Altitude() : SetSCD40Parameter(F("Set Sensor Altitude (m)")) {

    }

    void onValue(long value) override {
        getDevices()->scd40->setSensorAltitude(value);
    }
};

#endif //STALEMATE_SETSCD40ALTITUDE_H
