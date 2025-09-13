#include "ExplorIRData.h"
#include <Arduino.h>

ExplorIRData::ExplorIRData() { clear(); }

void ExplorIRData::clear()
{
    co2filtered = 0;
    co2unfiltered = 0;
    clearPPMScaling();
    clearPressureCompensation();
    clearCO2Calibration();
    clearNitrogenCalibration();
    clearAirCalibration();
    clearModuleInfo();
}

void ExplorIRData::clearModuleInfo()
{
    memset(firmwareDate, 0, sizeof(firmwareDate));
    memset(firmwareTime, 0, sizeof(firmwareTime));
    memset(firmwareVersion, 0, sizeof(firmwareVersion));
    memset(serialNumber, 0, sizeof(serialNumber));
}

void ExplorIRData::print() const
{
    Serial.printf("CO2: %u (%u)\r\n", getCO2PPMFiltered(), getCO2PPMUnfiltered());
}

void ExplorIRData::printModuleInfo() const
{
    Serial.printf(F("ExplorIR CO2 Sensor Info:\r\n"
                    "\t Date/Time : %s %s\r\n"
                    "\t Version   : %s\r\n"
                    "\t Serial    : %s\r\n\r\n"),
                  firmwareDate, firmwareTime,
                  firmwareVersion,
                  serialNumber);
}

int ExplorIRData::getCO2PPMFiltered() const
{
    return co2filtered * co2PPMScaling;
}

int ExplorIRData::getCO2PPMUnfiltered() const
{
    return co2unfiltered * co2PPMScaling;
}

float ExplorIRData::getCO2PercentFiltered() const
{
    return getCO2PPMFiltered() / (float)10000.0;
}

float ExplorIRData::getCO2PercentUnfiltered() const
{
    return getCO2PPMUnfiltered() / (float)10000.0;
}

