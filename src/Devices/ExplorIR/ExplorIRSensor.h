#pragma once
#include <Arduino.h>
#include "ExplorIRData.h"

class ExplorIRSensor
{
public:
    ExplorIRSensor();
    ~ExplorIRSensor();
    void init();
    void update();

    void setAltitudeM(int altitudeM);

    void calibrateInNitrogenArgon();
    void calibrateIn100PercentCO2();
    void calibrateInFreshAir();

    void setDigitalFilter(int newValue);

    int fetchPressureCompensation(bool startStopPolling = true);
    int fetchCO2PPMScaling(bool togglePolling = true);
    int fetchDigitalFilter(bool togglePolling = true);

    [[nodiscard]] const ExplorIRData* getData() const { return &data; }

    [[nodiscard]] static int getCompensationFromAltitudeM(int altitudeM);

    [[nodiscard]] unsigned long getNumSamples() const { return numSamples; }

private:
    void clearMessageData();
    const char* getPrintableMessageData();

    void stopPolling();
    void startPolling();

    enum MessageState
    {
        MESSAGE_STATE_NONE,
        MESSAGE_STATE_GET_TYPE,
        MESSAGE_STATE_GET_DATA,
        MESSAGE_STATE_DONE
    };
    MessageState messageState;
    char currentMessageType;
    char currentMessageData[128];
    uint8_t currentMessageDataIndex;

    bool processRXBuffer(char c);
    void processCompletedMessage();

    void busyWait();
    void sendCommand(const char* command);

    char printBuf[512];
    ExplorIRData data;
    int currentKValue = -1;
    int currentMValue = -1;
    uint8_t numYLines = 0;

    unsigned long numSamples = 0;

    const bool verbose = true;
};
