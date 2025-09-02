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

void SCD40_CO2::clear() {
    co2ppm = 0;
    temperatureC = 0;
    humidity = 0;
    memset(serialNumber, 0, sizeof(serialNumber));
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
    delay(50);

    Serial.println(F("SCD40: INIT: Running self test."));
    if(!runSelfTest()) {
        Serial.println(F("SCD40: INIT: Failed self test."));
        while(1) delay(10);
    }
    delay(50);

    Serial.print(F("SCD40: INIT: Setting altitude to: "));
    Serial.println((uint16_t)altitudeM);
    setSensorAltitude((uint16_t)altitudeM);
}

bool SCD40_CO2::runSelfTest() {
    bool success = false;

    if(!sensor.stopPeriodicMeasurement(500)) {
        CO2_DEBUG("CO2040");
    }
    else if(!sensor.performSelfTest()) {
        CO2_DEBUG("CO2041");
    }
    else if(!readChipInfo()) {
        CO2_DEBUG("CO2043");
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
    temperatureC = sensor.getTemperature();
    humidity = sensor.getHumidity();
}

bool SCD40_CO2::readChipInfo() {

    if(!sensor.getSerialNumber(serialNumber)) {
        Serial.println(F("SCD40: INIT: Failed to read serial number"));
        return false;
    }

    temperatureOffset = sensor.getTemperatureOffset();
    sensorAltitude = sensor.getSensorAltitude();

    scd4x_sensor_type_e sensorType = sensor.getSensorType();
    if(sensorType == SCD4x_SENSOR_SCD40) sensorTypeString = "SCD40";
    else if(sensorType == SCD4x_SENSOR_SCD41) sensorTypeString = "SCD41";
    else if(sensorType == SCD4x_SENSOR_INVALID) sensorTypeString = "INVALID";
    else sensorTypeString = "????";

    return true;
}

void SCD40_CO2::setSensorAltitude(uint16_t newAltitude) {
    if(!sensor.stopPeriodicMeasurement()) {
        Serial.println(F("SCD40: Failed to stop periodic measurement"));
    }
    else if(!sensor.setSensorAltitude(newAltitude, 500)) {
        Serial.println(F("SCD40: Failed to set altitude"));
    }
    else if(!readChipInfo()) {
        Serial.println(F("SCD40: Failed to read new chip info"));
    }
    else if(!sensor.startPeriodicMeasurement()) {
        Serial.println(F("SCD40: Failed to start periodic measurement"));
    }
    else {
        // great!
    }
}

void SCD40_CO2::setTempOffset(float tempOffset) {
    if(!sensor.stopPeriodicMeasurement()) {
        Serial.println(F("SCD40: Failed to stop periodic measurement"));
    }
    else if(!sensor.setTemperatureOffset(tempOffset, 500)) {
        Serial.println(F("SCD40: Failed to set temp offset"));
    }
    else if(!readChipInfo()) {
        Serial.println(F("SCD40: Failed to read new chip info"));
    }
    else if(!sensor.startPeriodicMeasurement()) {
        Serial.println(F("SCD40: Failed to start periodic measurement"));
    }
    else {
        // great!
    }
}

