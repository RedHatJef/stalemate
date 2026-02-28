//
// Created by redha on 9/19/2025.
//

#ifndef STALEMATE_SETRECORDMODETOEVERYUPDATE_H
#define STALEMATE_SETRECORDMODETOEVERYUPDATE_H

#include "Menu/MenuAction.h"

class SetRecordModeToEveryUpdate : public MenuAction {
public:
    SetRecordModeToEveryUpdate() : MenuAction(F("Save Every ExplorIR Update (2x second)")) {}

private:
    void doAction() override {
        getDevices()->eepromData->setRecordMode(RECORDMODE_EVERY_UPDATE);
    }
};

#endif //STALEMATE_SETRECORDMODETOEVERYUPDATE_H
