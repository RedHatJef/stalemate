//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_SCD40_CO2_H
#define STALEMATE_SCD40_CO2_H

#include <stdint.h>
#include "Util/Utils.h"

class SCD40_CO2 {
public:
    SCD40_CO2();
    void init(float altitudeM);
    void update();
    void clear();

    bool runSelfTest();

    [[nodiscard]] unsigned int getCO2PPM() const { return co2ppm; }
    [[nodiscard]] float getTemperatureC() const { return temperatureC; }
    [[nodiscard]] float getTemperatureF() const { return getFahrenheitFromCelsius(temperatureC); }
    [[nodiscard]] float getHumidity() const { return humidity; }

    [[nodiscard]] const char* getSerialNumber() const { return serialNumber; }
    [[nodiscard]] float getTemperatureOffset() const { return temperatureOffset; }
    [[nodiscard]] uint16_t getSensorAltitude() const { return sensorAltitude; }
    [[nodiscard]] const char* getSensorType() const { return sensorTypeString; }

    void setSensorAltitude(uint16_t newAltitude);
    void setTempOffset(float tempOffset);

private:
    bool readChipInfo();

    unsigned int co2ppm;
    float temperatureC;
    float humidity;
    char serialNumber[32];
    float temperatureOffset;
    uint16_t sensorAltitude;
    const char* sensorTypeString;
};


#endif //STALEMATE_SCD40_CO2_H
