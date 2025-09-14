//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_CALIBRATEINFRESHAIR_H
#define STALEMATE_CALIBRATEINFRESHAIR_H

#include "Menu/MenuAction.h"

class CalibrateInFreshAir : public MenuAction {
public:
    CalibrateInFreshAir() : MenuAction(F("Calibrate in Fresh Air")) { }

    void doAction() override {
        getDevices()->explorIR->calibrateInFreshAir();
    }
};

#endif //STALEMATE_CALIBRATEINFRESHAIR_H
