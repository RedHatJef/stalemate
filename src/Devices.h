//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_DEVICES_H
#define ETHAN1_DEVICES_H

#include "Clock.h"
#include "BMP.h"
#include "SHT.h"
#include "SCD40_CO2.h"

typedef struct Devices {
    const Clock* clock;
//    const BMP* bmp;
//    const SHT* sht;
//    const SCD40_CO2* scd40;
};

#endif //ETHAN1_DEVICES_H
