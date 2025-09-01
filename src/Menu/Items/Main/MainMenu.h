//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_MAINMENU_H
#define ETHAN1_MAINMENU_H

#include "Menu/Menu.h"
#include "Menu/Items/Time/TimeMenu.h"
#include "HelloWorld.h"

class MainMenu : public Menu {
public:
    MainMenu() : Menu(F("Main Menu")) {
        addMenuOption(&_hello);
        addMenuOption(&_timeMenu);

        _timeMenu.setParent(this);
    }
private:
    HelloWorld _hello;
    TimeMenu   _timeMenu;
};


#endif //ETHAN1_MAINMENU_H
