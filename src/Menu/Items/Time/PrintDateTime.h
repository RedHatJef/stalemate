//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_PRINTTIME_H
#define ETHAN1_PRINTTIME_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class PrintDateTime : public MenuAction {
public:
    PrintDateTime() : MenuAction("Print Date/Time") {}
    void doAction() override;
};


#endif //ETHAN1_PRINTTIME_H
