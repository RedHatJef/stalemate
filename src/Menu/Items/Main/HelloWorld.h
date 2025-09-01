//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_HELLOWORLD_H
#define ETHAN1_HELLOWORLD_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class HelloWorld : public MenuAction {
public:
    HelloWorld() : MenuAction(F("Hello World")) {}
    void doAction() override {
        Serial.println(F("Printing Hello World for all my fans.  :)"));
    }
};


#endif //ETHAN1_HELLOWORLD_H
