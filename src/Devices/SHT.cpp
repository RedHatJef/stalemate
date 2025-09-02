//
// Created by redha on 3/10/2025.
//

#include "SHT.h"
#include "Adafruit_SHT31.h"
#include "Utils.h"

#ifdef SHT31_ENABLE_DEBUGGING
#define SHT31_DEBUG(x) Serial.println(x)
#else
#define SHT31_DEBUG(x)
#endif

bool enableHeater = false;

uint8_t loopCnt = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();

SHT::SHT() {
    clear();
}

float SHT::getTempC() const {
    return tempC;
}

float SHT::getTempF() const {
    return getFahrenheitFromCelsius(getTempC());
}

float SHT::getHumidity() const {
    return humidity;
}

void SHT::init() {
    if(!sht31.begin()) {
        Serial.println(F("SHT31: Failed to start."));
        while(1) delay(10);
    }

    SHT31_DEBUG(sht31.isHeaterEnabled() ? F("SHT31 Heater ENABLED") : F("SHT31: Heater DISABLED"));
}

void SHT::clear() {
    tempC = 0;
    humidity = 0;
}

void SHT::update() {
    unsigned long nowTime = millis();
    if(nowTime - lastHeaterEnable > 30000) {
        lastHeaterEnable = nowTime;
        enableHeater = !enableHeater;
        sht31.heater(enableHeater);
        SHT31_DEBUG(sht31.isHeaterEnabled() ? "SHT31: ENABLED heater" : "SHT31: DISABLED heater");
    }

    tempC = sht31.readTemperature();

    if(!sht31.readBoth(&tempC, &humidity)) {
        Serial.println(F("SHT31: Failed to read sensor."));
        clear();
        return;
    }

    if(isnan(tempC)) {
        Serial.println(F("SHT31: Invalid temp."));
        clear();
        return;
    }

    if(isnan(humidity)) {
        Serial.println(F("SHT31: Invalid humidity."));
        clear();
        return;
    }
}
