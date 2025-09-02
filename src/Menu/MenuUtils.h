//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_MENUUTILS_H
#define STALEMATE_MENUUTILS_H

#include "MenuActionInput.h"
#include "Devices/Storage.h"

extern void EnterInputMode(MenuActionInput* action);
extern void beginIntegerInput(MenuActionInput* action);
extern void resetInputBuffer();
extern void finishIntegerInput();
extern void displayMenu();
extern void handleMenuChoice(uint8_t digit);
extern void processSerialInput();
extern void setMenuDevices(Devices* devices);
extern void setMenuStorage(Storage* storage);

#endif //STALEMATE_MENUUTILS_H
