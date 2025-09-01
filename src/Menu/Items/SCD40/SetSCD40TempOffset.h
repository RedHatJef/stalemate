//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETSCD40TEMPOFFSET_H
#define STALEMATE_SETSCD40TEMPOFFSET_H

#include "SetSCD40Parameter.h"

class SetSCD40TempOffset : public SetSCD40Parameter {
public:
    SetSCD40TempOffset() : SetSCD40Parameter(F("Set Sensor Temp Offset (C)")) {

    }

    void onValue(long value) override {
        float fValue = (float)value;
        Serial.print(F("Setting temp offset to "));
        Serial.println(fValue);
        Serial.println(value);
        getDevices()->scd40->setTempOffset(fValue);
    }
};

#endif //STALEMATE_SETSCD40TEMPOFFSET_H
