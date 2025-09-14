//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_GETPRESSURECOMPENSATION_H
#define STALEMATE_GETPRESSURECOMPENSATION_H

#include "Menu/MenuAction.h"

class GetPressureCompensation : public MenuAction {
public:
    GetPressureCompensation() : MenuAction(F("Get Pressure Compensation Value")) { }

    void doAction() override {
        getDevices()->explorIR->fetchPressureCompensation();
    }

private:
};

#endif //STALEMATE_GETPRESSURECOMPENSATION_H
