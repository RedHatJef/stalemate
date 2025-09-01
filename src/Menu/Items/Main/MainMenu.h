//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_MAINMENU_H
#define STALEMATE_MAINMENU_H

#include "Menu/Menu.h"
#include "Menu/Items/Time/TimeMenu.h"
#include "Menu/Items/Storage/StorageMenu.h"
#include "Menu/Items/BMP390/BMP390Menu.h"

class MainMenu : public Menu {
public:
    MainMenu() : Menu(F("Main Menu")) {
        addMenuOption(&timeMenu);
        addMenuOption(&storageMenu);
        addMenuOption(&bmp390Menu);

        timeMenu.setParent(this);
        storageMenu.setParent(this);
        bmp390Menu.setParent(this);
    }

    void setStorage(Storage* s) {
        storageMenu.setStorage(s);
    }

private:
    TimeMenu timeMenu;
    StorageMenu storageMenu;
    BMP390Menu bmp390Menu;
};

#endif //STALEMATE_MAINMENU_H
