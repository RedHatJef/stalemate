//
// Created by redha on 9/2/2025.
//

#ifndef STALEMATE_EXPLORIRVERSIONINFO_H
#define STALEMATE_EXPLORIRVERSIONINFO_H

#include "Menu/MenuAction.h"

class ExplorIRVersionInfo : public MenuAction {
public:
    ExplorIRVersionInfo() : MenuAction(F("Print Firmware Version & Serial Number")) { }

    void doAction() override {
        Serial.println(F("ExplorIR CO2 Sensor: "));
        ExplorIRCO2::VersionInfo v;
        getDevices()->explorIR->getVersionInfo(v);
        Serial.print(F("  MFG Date  = "));
        printCharacterBufferHex(v.fwDateTime, sizeof(v.fwDateTime));
        Serial.print(F("  Revision  = "));
        printCharacterBufferHex(v.fwRev, sizeof(v.fwRev));
        Serial.print(F("  Sensor ID = "));
        printCharacterBufferHex(v.sensorID, sizeof(v.sensorID));
    }

private:
    void printCharacterBufferHex(const char* buf, uint8_t length) {
        char temp[4];
        for(uint8_t i = 0; i < length; i++) {
            memset(temp, 0, sizeof(temp));
            char* c = itoa(buf[i], temp, 16);
            Serial.print(c);
        }
    }
};

#endif //STALEMATE_EXPLORIRVERSIONINFO_H
