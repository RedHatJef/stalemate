//
// Created by redha on 9/19/2025.
//

#ifndef STALEMATE_SETRECORDMODESECONDSINTERVAL_H
#define STALEMATE_SETRECORDMODESECONDSINTERVAL_H

#include "Menu/MenuActionInput.h"
#include "Menu/MenuUtils.h"

class SetRecordModeSecondsInterval : public MenuActionInput {
public:
    SetRecordModeSecondsInterval() : MenuActionInput(F("Save Data Every X Seconds")) {}

    void doAction() override {
        // Enter input capture for this instance
        EnterInputMode(this);
    }

    void onValue(long value) override {
        getDevices()->eepromData->setRecordMode(RECORDMODE_SECONDS);
        getDevices()->eepromData->setRecordIntervalSeconds(value);
    }
};

#endif //STALEMATE_SETRECORDMODESECONDSINTERVAL_H
