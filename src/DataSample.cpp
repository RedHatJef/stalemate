//
// Created by redha on 3/12/2025.
//

#include <string.h>
#include "DataSample.h"

const char* DataSample::tableHeader = "date,time,bmp390TempC,sht31TempC,sht31Humidity,scd40co2ppm";

DataSample::DataSample() {
    clear();
}

void DataSample::clear() {
    memset(date, 0, sizeof(date));
    memset(time, 0, sizeof(time));
    bmp390TempC = 0;
    sht31TempC = 0;
    sht31HumidityPercent = 0;
    co2PPM = 0;
}
