//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_TIMEMENU_H
#define ETHAN1_TIMEMENU_H

#include "Menu/Menu.h"
#include "PrintDateTime.h"
#include "SetHours.h"
#include "SetMinutes.h"
#include "SetSeconds.h"
#include "SetYear.h"
#include "SetMonth.h"
#include "SetDay.h"

class TimeMenu : public Menu {
public:
    TimeMenu() : Menu("Time Settings") {
        addMenuOption(&printDateTime);
        addMenuOption(&setYear);
        addMenuOption(&setMonth);
        addMenuOption(&setDay);
        addMenuOption(&setHours);
        addMenuOption(&setMinutes);
        addMenuOption(&setSeconds);
    }

private:
    PrintDateTime printDateTime;
    SetYear setYear;
    SetMonth setMonth;
    SetDay setDay;
    SetHours setHours;
    SetMinutes setMinutes;
    SetSeconds setSeconds;
};


#endif //ETHAN1_TIMEMENU_H
