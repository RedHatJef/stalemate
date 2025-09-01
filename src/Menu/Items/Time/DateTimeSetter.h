//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_DATETIMESETTER_H
#define STALEMATE_DATETIMESETTER_H

#include "Menu/MenuActionInput.h"
#include "Menu/MenuUtils.h"

class DateTimeSetter : public MenuActionInput {
public:
    DateTimeSetter(const char* title) : MenuActionInput(title) { }

    void doAction() override {
        // Enter input capture for this instance
        EnterInputMode(this);
    }

protected:
    void printDateTime() const {
        getDevices()->clock->update();
        Serial.print(F("Current Date/Time: "));
        Serial.print(getDevices()->clock->getDateString());
        Serial.print("   ");
        Serial.println(getDevices()->clock->getTimeString());
    }
};


#endif //STALEMATE_DATETIMESETTER_H
