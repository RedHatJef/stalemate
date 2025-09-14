//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_CALIBRATEINNITROGENARGON_H
#define STALEMATE_CALIBRATEINNITROGENARGON_H

#include "Menu/MenuAction.h"

class CalibrateInNitrogenArgon : public MenuAction {
public:
    CalibrateInNitrogenArgon() : MenuAction(F("Calibrate in Nitrogen/Argon")) { }

    void doAction() override {
        getDevices()->explorIR->calibrateInNitrogenArgon();
    }
};

#endif //STALEMATE_CALIBRATEINNITROGENARGON_H
