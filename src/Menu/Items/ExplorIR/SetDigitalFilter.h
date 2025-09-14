//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_SETDIGITALFILTER_H
#define STALEMATE_SETDIGITALFILTER_H

#include "Menu/MenuAction.h"
#include "Menu/MenuActionInput.h"
#include "Menu/MenuUtils.h"

class SetDigitalFilter : public MenuActionInput {
public:
    SetDigitalFilter() : MenuActionInput(F("Set Digital Filter")) { }

    void doAction() override {
        // Enter input capture for this instance
        EnterInputMode(this);
    }

    void onValue(long value) override {
        getDevices()->explorIR->setDigitalFilter((int)value);

        Serial.println(F("Re-fetching digital filter value."));
        getDevices()->explorIR->fetchDigitalFilter();
    }
};

#endif //STALEMATE_SETDIGITALFILTER_H
