//
// Created by redha on 9/14/2025.
//

#include "DataSample.h"

const char* DataSample::tableHeader =
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
    "\n";

const char* DataSample::logFormat =
    "%f,%f,%f,"
    "%f,%f,"
    "%u,%f,"
    "%d,%d,%lu\n";

DataSample::DataSample() {
    clear();
}

void DataSample::clear() {
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

const char *DataSample::getDataString(char *buf, size_t len) const {

    memset(buf, 0, len);

    strcat(buf, date);
    strcat(buf, ",");
    strcat(buf, time);
    strcat(buf, ",");

    writeField(buf, bmp390AltitudeM);
    strcat(buf, ",");
    writeField(buf, bmp390TempC);
    strcat(buf, ",");
    writeField(buf, bmp390PresHPA);
    strcat(buf, ",");

    writeField(buf, sht31TempC);
    strcat(buf, ",");
    writeField(buf, sht31Humidity);
    strcat(buf, ",");

    writeField(buf, scd40CO2PPM);
    strcat(buf, ",");
    writeField(buf, scd40Humidity);
    strcat(buf, ",");

    writeField(buf, explorIRCO2Filtered);
    strcat(buf, ",");
    writeField(buf, explorIRCO2Unfiltered);
    strcat(buf, ",");
    writeField(buf, explorIRNumSamples);
    strcat(buf, "\n");

//    Serial.printf("DataSample: (len=%d) --> ", strlen(buf));
//    Serial.println(buf);

    return buf;
}

void DataSample::setFromDevices(const Devices *d) {
    clear();

    strcpy(date, d->clock->getDateString());
    strcpy(time, d->clock->getTimeString());

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

