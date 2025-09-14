//
// Created by redha on 3/12/2025.
//

#ifndef STALEMATE_DATASAMPLE_H
#define STALEMATE_DATASAMPLE_H

#include "Devices.h"

class DataSample {
public:
    DataSample();

    void setFromDevices(const Devices* d);
    const char* getDataString(char* buf, size_t len) const;
    void clear();

    static const char* tableHeader;

private:

    static void writeField(char* buffer, float f) {
        char b[32];
        snprintf(b, sizeof(b), "%3.3f", f);
        strcat(buffer, b);
    }

    static void writeField(char* buffer, int i) {
        char b[32];
        snprintf(b, sizeof(b), "%d", i);
        strcat(buffer, b);
    }

    static void writeField(char* buffer, unsigned int i) {
        char b[32];
        snprintf(b, sizeof(b), "%u", i);
        strcat(buffer, b);
    }

    static void writeField(char* buffer, unsigned long i) {
        char b[32];
        snprintf(b, sizeof(b), "%lu", i);
        strcat(buffer, b);
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

    static const char* logFormat;
};

#endif //STALEMATE_DATASAMPLE_H
