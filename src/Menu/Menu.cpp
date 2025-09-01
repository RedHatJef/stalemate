//
// Created by redha on 9/1/2025.
//

#include "Menu.h"

void Menu::setDevices(Devices *devices) {
    MenuOption::setDevices(devices);
    for(uint8_t i = 0; i < MENU_MAX_ITEMS; i++) {
        MenuOption* thisOption = _items[i];
        if(thisOption == nullptr) continue;
        thisOption->setDevices(devices);
    }
}

