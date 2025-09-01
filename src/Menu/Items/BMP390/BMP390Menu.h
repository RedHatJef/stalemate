//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_BMP390MENU_H
#define STALEMATE_BMP390MENU_H

#include "Menu/Menu.h"
#include "PrintBMPInfo.h"

class BMP390Menu : public Menu {
public:
    BMP390Menu() : Menu(F("BMP390 Pressure/Altimeter Options")) {
        addMenuOption(&printBmpInfo);
    }

private:
    PrintBMPInfo printBmpInfo;
};


#endif //STALEMATE_BMP390MENU_H
