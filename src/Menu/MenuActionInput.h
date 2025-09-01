//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_MENUACTIONINPUT_H
#define STALEMATE_MENUACTIONINPUT_H

#include <Arduino.h>
#include "MenuAction.h"

class MenuActionInput : public MenuAction {
public:
    using MenuAction::MenuAction;
    // Called after the user hits Enter with a parsed long.
    virtual void onValue(long value) = 0;

    void doAction() override {
        startInput();
    }

    // Called by the global input handler when a complete integer is captured
    void submitValue(long v) {
        onValue(v);
    }

    void startInput() {
        Serial.print(getTitle());
        Serial.println(F(": Enter integer value (ms). Press Enter when done."));
        Serial.print(F("> "));
    }
};


#endif //STALEMATE_MENUACTIONINPUT_H
