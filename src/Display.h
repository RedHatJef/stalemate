//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_DISPLAY_H
#define STALEMATE_DISPLAY_H


#include "Devices.h"

class Display {
public:
    Display();

    void init(const Devices* d);
    void update();
    void clear();
    void println(const __FlashStringHelper *s);

private:
    const Devices* devices;
};


#endif //STALEMATE_DISPLAY_H
