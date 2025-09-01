//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_HELLOWORLD_H
#define ETHAN1_HELLOWORLD_H

#include <Arduino.h>
#include "Menu/MenuAction.h"

class HelloWorld : public MenuAction {
public:
    HelloWorld() : MenuAction("Hello World") {}
    void doAction() override {
        Serial.println(F("Hello World"));
    }
};


#endif //ETHAN1_HELLOWORLD_H
