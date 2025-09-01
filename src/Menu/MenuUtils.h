//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_MENUUTILS_H
#define ETHAN1_MENUUTILS_H


#include "MenuActionInput.h"

extern void EnterInputMode(MenuActionInput* action);
extern void beginIntegerInput(MenuActionInput* action);
extern void resetInputBuffer();
extern void finishIntegerInput();
extern void displayMenu();
extern void handleMenuChoice(uint8_t digit);
extern void processSerialInput();

#endif //ETHAN1_MENUUTILS_H
