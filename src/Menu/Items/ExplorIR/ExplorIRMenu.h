//
// Created by redha on 9/2/2025.
//

#ifndef STALEMATE_EXPLORIRMENU_H
#define STALEMATE_EXPLORIRMENU_H

#include "Menu/Menu.h"
#include "ExplorIRVersionInfo.h"

class ExplorIRMenu : public Menu {
public:
    ExplorIRMenu() : Menu(F("ExplorIR Options (CO2)")) {
        addMenuOption(&explorIrVersionInfo);
    }

private:
    ExplorIRVersionInfo explorIrVersionInfo;
};

#endif //STALEMATE_EXPLORIRMENU_H
