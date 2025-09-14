//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_GETDIGITALFILTER_H
#define STALEMATE_GETDIGITALFILTER_H

#include "Menu/MenuAction.h"

class GetDigitalFilter : public MenuAction {
public:
    GetDigitalFilter() : MenuAction(F("Get Digital Filter Value")) { }

    void doAction() override {
        getDevices()->explorIR->fetchDigitalFilter();
    }

private:
};

#endif //STALEMATE_GETDIGITALFILTER_H
