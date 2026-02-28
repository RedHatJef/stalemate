//
// Created by redha on 9/19/2025.
//

#ifndef STALEMATE_RECORDOPTIONMENU_H
#define STALEMATE_RECORDOPTIONMENU_H

#include "Menu/Menu.h"
#include "PrintCurrentUpdateMode.h"
#include "SetRecordModeToEveryUpdate.h"
#include "SetRecordModeSecondsInterval.h"

class RecordOptionMenu : public Menu {
public:
    RecordOptionMenu() : Menu(F("Record Options")) {
        addMenuOption(&printCurrentUpdateMode);
        addMenuOption(&setRecordModeToEveryUpdate);
        addMenuOption(&setRecordModeSecondsInterval);
    }

private:
    PrintCurrentUpdateMode printCurrentUpdateMode;
    SetRecordModeToEveryUpdate setRecordModeToEveryUpdate;
    SetRecordModeSecondsInterval setRecordModeSecondsInterval;
};

#endif //STALEMATE_RECORDOPTIONMENU_H
