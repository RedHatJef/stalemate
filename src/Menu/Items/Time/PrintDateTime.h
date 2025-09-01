//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_PRINTTIME_H
#define STALEMATE_PRINTTIME_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class PrintDateTime : public MenuAction {
public:
    PrintDateTime() : MenuAction(F("Print Date/Time")) {}
    void doAction() override;
};


#endif //STALEMATE_PRINTTIME_H
