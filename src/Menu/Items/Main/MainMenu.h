//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_MAINMENU_H
#define STALEMATE_MAINMENU_H

#include "Menu/Menu.h"
#include "Menu/Items/Time/TimeMenu.h"
#include "Menu/Items/Storage/StorageMenu.h"

class MainMenu : public Menu {
public:
    MainMenu() : Menu(F("Main Menu")) {
        addMenuOption(&timeMenu);
        addMenuOption(&storageMenu);

        timeMenu.setParent(this);
        storageMenu.setParent(this);
    }

    void setStorage(Storage* s) {
        storageMenu.setStorage(s);
    }

private:
    TimeMenu timeMenu;
    StorageMenu storageMenu;
};


#endif //STALEMATE_MAINMENU_H
