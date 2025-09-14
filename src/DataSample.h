//
// Created by redha on 3/12/2025.
//

#ifndef STALEMATE_DATASAMPLE_H
#define STALEMATE_DATASAMPLE_H

#include "Devices.h"

class DataSample {
public:
    DataSample()
    {
        clear();
    }

    void setFromDevices(const Devices* d)
    {
        clear();

        bmp390AltitudeM = d->bmp->getAltitudeM();
        bmp390TempC = d->bmp->getTempC();
        bmp390PresHPA = d->bmp->getPresHPA();

        sht31TempC = d->sht->getTempC();
        sht31Humidity = d->sht->getHumidity();

        scd40CO2PPM = d->scd40->getCO2PPM();
        scd40Humidity = d->scd40->getHumidity();

        explorIRCO2Filtered = d->explorIR->getData()->getCO2PPMFiltered();
        explorIRCO2Unfiltered = d->explorIR->getData()->getCO2PPMUnfiltered();
        explorIRNumSamples = d->explorIR->getNumSamples();
    }

    const char* getDataString(char* buf, size_t len)
    {
        snprintf(buf, len, logFormat,
                 bmp390AltitudeM,
                 bmp390TempC,
                 bmp390PresHPA,

                 sht31TempC,
                 sht31Humidity,

                 scd40CO2PPM,
                 scd40Humidity,

                 explorIRCO2Filtered,
                 explorIRCO2Unfiltered,
                 explorIRNumSamples
                 );

        return buf;
    }

    char date[11]; // 03/12/2025 = 10 + null = 11
    char time[9]; // 18:12:34 = 8 + null = 9

    float bmp390AltitudeM;
    float bmp390TempC;
    float bmp390PresHPA;

    float sht31TempC;
    float sht31Humidity;

    unsigned int scd40CO2PPM;
    float scd40Humidity;

    int explorIRCO2Filtered;
    int explorIRCO2Unfiltered;
    unsigned long explorIRNumSamples;

    void clear() {
        memset(date, 0, sizeof(date));
        memset(time, 0, sizeof(time));

        bmp390AltitudeM = 0;
        bmp390TempC = 0;
        bmp390PresHPA = 0;

        sht31TempC = 0;
        sht31Humidity = 0;

        scd40CO2PPM = 0;
        scd40Humidity = 0;

        explorIRCO2Filtered = 0;
        explorIRCO2Unfiltered = 0;
        explorIRNumSamples = 0;
    }

    const char* tableHeader =
            "date,"
            "time,"
            "bmp390AltitudeM,"
            "bmp390TempC,"
            "bmp390PresHPA,"
            "sht31TempC,"
            "sht31Humidity,"
            "scd40CO2PPM,"
            "scd40Humidity,"
            "explorIRCO2Filtered,"
            "explorIRCO2Unfiltered,"
            "explorIRNumSamples"
            "";

private:
    const char* logFormat = "%f,%f,%f,"
                            "%f,%f,"
                            "%u,%f,"
                            "%d,%d,%lu\n";
};


#endif //STALEMATE_DATASAMPLE_H
