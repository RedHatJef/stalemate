#include "ExplorIRData.h"

#include <iostream>

void ExplorIRData::clear()
{
    co2filtered = 0;
    co2unfiltered = 0;
    clearPPMScaling();
    clearPressureCompensation();
    clearCO2Calibration();
    clearNitrogenCalibration();
    clearAirCalibration();
    memset(firmwareDate, 0, sizeof(firmwareDate));
    memset(firmwareTime, 0, sizeof(firmwareTime));
    memset(firmwareVersion, 0, sizeof(firmwareVersion));
    memset(serialNumber, 0, sizeof(serialNumber));
}

void ExplorIRData::print() const
{
    std::cout << "CO2: " << getCO2PPMFiltered() << "  ( " << getCO2PPMUnfiltered() << " )" << "\n";
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
    return getCO2PPMFiltered() / 10000.0;
}

float ExplorIRData::getCO2PercentUnfiltered() const
{
    return getCO2PPMUnfiltered() / 10000.0;
}
