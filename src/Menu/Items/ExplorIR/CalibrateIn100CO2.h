//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_CALIBRATEIN100CO2_H
#define STALEMATE_CALIBRATEIN100CO2_H

#include "Menu/MenuAction.h"

class CalibrateIn100CO2 : public MenuAction {
public:
    CalibrateIn100CO2() : MenuAction(F("Calibrate in 100% CO2")) { }

    void doAction() override {
        getDevices()->explorIR->calibrateIn100PercentCO2();
    }
};

#endif //STALEMATE_CALIBRATEIN100CO2_H
