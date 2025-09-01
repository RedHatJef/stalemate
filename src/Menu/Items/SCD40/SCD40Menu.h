//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SCD40MENU_H
#define STALEMATE_SCD40MENU_H

#include "Menu/Menu.h"
#include "PrintSCD40Info.h"
#include "SetSCD40Altitude.h"
#include "SetSCD40TempOffset.h"

class SCD40Menu : public Menu {
public:
    SCD40Menu() : Menu(F("SCD40  Options (CO2, Temp, Humidity)")) {
        addMenuOption(&printScd40Info);
        addMenuOption(&setScd40Altitude);
        addMenuOption(&setScd40TempOffset);
    }

private:
    PrintSCD40Info printScd40Info;
    SetSCD40Altitude setScd40Altitude;
    SetSCD40TempOffset setScd40TempOffset;
};


#endif //STALEMATE_SCD40MENU_H
