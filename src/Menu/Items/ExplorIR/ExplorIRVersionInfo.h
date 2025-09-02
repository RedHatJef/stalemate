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
    }

private:
};

#endif //STALEMATE_EXPLORIRVERSIONINFO_H
