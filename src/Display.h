//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_DISPLAY_H
#define ETHAN1_DISPLAY_H


#include "Devices.h"

class Display {
public:
    Display();

    void init(const Devices* d);
    void update();

private:
    const Devices* devices;
};


#endif //ETHAN1_DISPLAY_H
