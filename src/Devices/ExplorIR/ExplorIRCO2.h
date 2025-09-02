// ExplorIRCO2_NonBlocking_NoHeap_Fixed128.h
// AVR128DA32 (DxCore) compatible; no Arduino String; no malloc/new; non-blocking update()

#pragma once
#include <Arduino.h>
#include "ExplorIRQueue.h"
#include "ExplorIRRunMode.h"

class ExplorIRCO2 {
public:
    explicit ExplorIRCO2(HardwareSerial& port = Serial2);

    void init();
    void update();

    [[nodiscard]] int getFilteredCO2PPM() const { return filteredCO2PPM * ppmScaleFactor; }
    [[nodiscard]] int getUnfilteredCO2PPM() const { return unfilteredCO2PPM * ppmScaleFactor; }

private:
    void addReceivedChar(char c);
    void processMessage(const char* buf, uint8_t messageSize);

    HardwareSerial& serial;
    ExplorIRQueue rxBuf;

    int digitalFilter = 0;
    int zeroPointSetting = 0;
    int zeroPointInAir = 0;
    int numMeasurementDataTypes = 0;
    int filteredCO2PPM = 0;
    int unfilteredCO2PPM = 0;
    int ppmScaleFactor = 0;
    ExplorIRRunMode currentMode = MODE_COMMAND_SLEEP;
};