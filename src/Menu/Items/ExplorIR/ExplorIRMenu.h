//
// Created by redha on 9/2/2025.
//

#ifndef STALEMATE_EXPLORIRMENU_H
#define STALEMATE_EXPLORIRMENU_H

#include "Menu/Menu.h"
#include "ExplorIRVersionInfo.h"
#include "GetPressureCompensation.h"
#include "SetPressureCompensation.h"
#include "GetDigitalFilter.h"
#include "SetDigitalFilter.h"
#include "CalibrateIn100CO2.h"
#include "CalibrateInNitrogenArgon.h"
#include "CalibrateInFreshAir.h"

class ExplorIRMenu : public Menu {
public:
    ExplorIRMenu() : Menu(F("ExplorIR Options (CO2)")) {
        addMenuOption(&explorIrVersionInfo);
        addMenuOption(&getPressureCompensation);
        addMenuOption(&setPressureCompensation);
        addMenuOption(&getDigitalFilter);
        addMenuOption(&setDigitalFilter);
        addMenuOption(&calibrateIn100Co2);
        addMenuOption(&calibrateInNitrogenArgon);
        addMenuOption(&calibrateInFreshAir);
    }

private:
    ExplorIRVersionInfo explorIrVersionInfo;
    GetPressureCompensation getPressureCompensation;
    SetPressureCompensation setPressureCompensation;
    GetDigitalFilter getDigitalFilter;
    SetDigitalFilter setDigitalFilter;
    CalibrateIn100CO2 calibrateIn100Co2;
    CalibrateInNitrogenArgon calibrateInNitrogenArgon;
    CalibrateInFreshAir calibrateInFreshAir;
};

#endif //STALEMATE_EXPLORIRMENU_H
