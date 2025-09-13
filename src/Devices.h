//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_DEVICES_H
#define STALEMATE_DEVICES_H

#include "Devices/Clock.h"
#include "Devices/BMP.h"
#include "Devices/SHT.h"
#include "Devices/SCD40_CO2.h"
#include "Devices/ExplorIR/ExplorIRSensor.h"

typedef struct Devices {
    Clock* clock;
    const BMP* bmp;
    const SHT* sht;
    SCD40_CO2* scd40;
    ExplorIRSensor* explorIR;
};

#endif //STALEMATE_DEVICES_H
