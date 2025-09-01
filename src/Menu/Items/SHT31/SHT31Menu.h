//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SHT31MENU_H
#define STALEMATE_SHT31MENU_H

#include "Menu/Menu.h"
#include "PrintSHT31Info.h"

class SHT31Menu : public Menu {
public:
    SHT31Menu() : Menu(F("SHT31  Options (Temperature/Humidity)")) {
        addMenuOption(&printSht31Info);
    }
    
private:
    PrintSHT31Info printSht31Info;
};


#endif //STALEMATE_SHT31MENU_H
