//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_TIMEMENU_H
#define ETHAN1_TIMEMENU_H

#include "Menu/Menu.h"
#include "PrintTime.h"
#include "TimeOffsetActionWithHook.h"
#include "TimeOffsetAction.h"

class TimeMenu : public Menu {
public:
    TimeMenu() : Menu("Time") {
        addMenuOption(&printTime);
        addMenuOption(&setOffset);
    }
private:
    PrintTime printTime;
    TimeOffsetAction setOffset;
};


#endif //ETHAN1_TIMEMENU_H
