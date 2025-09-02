//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_MAINMENU_H
#define STALEMATE_MAINMENU_H

#include "Menu/Menu.h"
#include "Menu/Items/Time/TimeMenu.h"
#include "Menu/Items/Storage/StorageMenu.h"
#include "Menu/Items/BMP390/BMP390Menu.h"
#include "Menu/Items/SHT31/SHT31Menu.h"
#include "Menu/Items/SCD40/SCD40Menu.h"
#include "Menu/Items/ExplorIR/ExplorIRMenu.h"

class MainMenu : public Menu {
public:
    MainMenu() : Menu(F("Main Menu")) {
        addMenuOption(&timeMenu);
        addMenuOption(&storageMenu);
        addMenuOption(&bmp390Menu);
        addMenuOption(&sht31Menu);
        addMenuOption(&scd40Menu);
        addMenuOption(&explorIrMenu);

        timeMenu.setParent(this);
        storageMenu.setParent(this);
        bmp390Menu.setParent(this);
        sht31Menu.setParent(this);
        scd40Menu.setParent(this);
        explorIrMenu.setParent(this);
    }

    void setStorage(Storage* s) {
        storageMenu.setStorage(s);
    }

private:
    TimeMenu timeMenu;
    StorageMenu storageMenu;
    BMP390Menu bmp390Menu;
    SHT31Menu sht31Menu;
    SCD40Menu scd40Menu;
    ExplorIRMenu explorIrMenu;
};

#endif //STALEMATE_MAINMENU_H
