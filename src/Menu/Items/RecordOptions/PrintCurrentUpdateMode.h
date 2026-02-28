//
// Created by redha on 9/19/2025.
//

#ifndef STALEMATE_PRINTCURRENTUPDATEMODE_H
#define STALEMATE_PRINTCURRENTUPDATEMODE_H

#include <Arduino.h>
#include "Devices/Storage/Storage.h"
#include "Menu/MenuAction.h"


class PrintCurrentUpdateMode : public MenuAction {
public:
    PrintCurrentUpdateMode() : MenuAction(F("Print Current Record Save Mode")) {}

private:
    void doAction() override {
        const char* s = getDevices()->eepromData->getRecordModeString();
        Serial.println(s);
    }
};

#endif //STALEMATE_PRINTCURRENTUPDATEMODE_H
