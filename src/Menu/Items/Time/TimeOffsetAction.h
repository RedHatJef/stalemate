//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_TIMEOFFSETACTION_H
#define ETHAN1_TIMEOFFSETACTION_H

#include <Arduino.h>
#include "Menu/MenuActionInput.h"
#include "Menu/MenuUtils.h"


// A specific input action: sets the global time offset in ms
class TimeOffsetAction : public MenuActionInput {
public:
    TimeOffsetAction() : MenuActionInput("Set Time Offset (ms)") {}
    void onValue(long value) override {
        gTimeOffsetMs = value;
        Serial.print(F("Time offset set to "));
        Serial.print(gTimeOffsetMs);
        Serial.println(F(" ms."));
    }

    void doAction() override {
        // Enter input capture for this instance
        EnterInputMode(this);
    }

protected:
    long gTimeOffsetMs;
};


#endif //ETHAN1_TIMEOFFSETACTION_H
