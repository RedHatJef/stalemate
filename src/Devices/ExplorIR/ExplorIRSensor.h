#pragma once
#include <Arduino.h>
#include "ExplorIRData.h"
#include "Util/SimpleQueue.h"

class ExplorIRSensor
{
public:
    ExplorIRSensor();
    ~ExplorIRSensor();
    void init();
    void update();

    void setAltitudeM(int altitudeM);
    void zeroInNitrogen();
    void zeroIn100PercentCO2();
    void calibrateInAir();

    const ExplorIRData* getData() const { return &data; }

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

    bool processRXBuffer();
    void processCompletedMessage();

    void busyWait();
    void sendCommand(const char* command);

    SimpleQueue<char, 128> rxBuf;

    char printBuf[512];
    ExplorIRData data;
    int currentKValue = -1;
    int currentMValue = -1;
    uint8_t numYLines = 0;

    const bool verbose = true;
};
