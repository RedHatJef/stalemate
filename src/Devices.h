//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_DEVICES_H
#define STALEMATE_DEVICES_H

#include "Clock.h"
#include "BMP.h"
#include "SHT.h"
#include "SCD40_CO2.h"

typedef struct Devices {
    Clock* clock;
//    const BMP* bmp;
//    const SHT* sht;
//    const SCD40_CO2* scd40;
};

#endif //STALEMATE_DEVICES_H
