//
// Created by redha on 3/10/2025.
//

#include "SCD40_CO2.h"
#include "SparkFun_SCD4x_Arduino_Library.h"

#ifdef CO2_ENABLE_DEBUGGING
#define CO2_DEBUG(x) Serial.println(x)
#else
#define CO2_DEBUG(x)
#endif

SCD4x sensor;

SCD40_CO2::SCD40_CO2() {
    clear();
}

unsigned int SCD40_CO2::getCO2PPM() const {
    return co2ppm;
}

void SCD40_CO2::clear() {
    co2ppm = 0;
}

void SCD40_CO2::init(float altitudeM) {
    if (!sensor.stopPeriodicMeasurement())
    {
        Serial.println(F("SCD40: INIT: Failed to stop."));
        while(1) delay(10);
    }

    //Now we can call .begin and set skipStopPeriodicMeasurements to true
    if (!sensor.begin(true, true, true))
        //measBegin_________/     |     |
        //autoCalibrate__________/      |
        //skipStopPeriodicMeasurements_/
    {
        Serial.println(F("SCD40: INIT: Failed to begin."));
        while(1) delay(10);
    }

    Serial.print(F("SCD40: INIT: Setting altitude to: "));
    Serial.println((uint16_t)altitudeM);
    sensor.setSensorAltitude((uint16_t)altitudeM);
    delay(50);

    Serial.println(F("SCD40: INIT: Running self test."));
    if(!runSelfTest()) {
        Serial.println(F("SCD40: INIT: Failed self test."));
        while(1) delay(10);
    }
}

bool SCD40_CO2::runSelfTest() {
    bool success = false;

    if(!sensor.stopPeriodicMeasurement(500)) {
        CO2_DEBUG("CO2040");
    }
    else if(!sensor.performSelfTest()) {
        CO2_DEBUG("CO2041");
    }
    else if(!sensor.startPeriodicMeasurement()) {
        CO2_DEBUG("CO2042");
    }
    else {
        success = true;
    }

    return success;
}

void SCD40_CO2::update() {
    co2ppm = sensor.getCO2();
}
