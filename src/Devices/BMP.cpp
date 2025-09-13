//
// Created by redha on 3/10/2025.
//

#include "BMP.h"
#include "Adafruit_BMP3XX.h"
#include "Util/Utils.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

BMP::BMP() {
    clear();
}

void BMP::clear() {
    tempC = 0;
    presHPA = 0;
    altitudeM = 0;
}

float BMP::getTempC() const {
    return tempC;
}

float BMP::getPresHPA() const {
    return presHPA;
}

float BMP::getAltitudeM() const {
    return altitudeM;
}

float BMP::getTempF() const {
    return getFahrenheitFromCelsius(getTempC());
}

float BMP::getAltitudeF() const {
    return getFeetFromMeters(getAltitudeM());
}

void BMP::init() {
    if(!bmp.begin_I2C()) {
        Serial.println(F("Failed to start BMP390"));
        while(1) delay(10);
    }

    chipId = bmp.chipID();

    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_32X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_32X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void BMP::update() {
    if(!bmp.performReading()) {
        Serial.println(F("Failed to read bmp390"));
        return;
    }

    tempC = bmp.temperature;
    presHPA = bmp.pressure;
    altitudeM = bmp.readAltitude(SEALEVELPRESSURE_HPA);
}
