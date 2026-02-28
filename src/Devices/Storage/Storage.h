//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_STORAGE_H
#define STALEMATE_STORAGE_H

#include "Devices.h"
#include "DataSample.h"
#include "StorageFileName.h"

#define SAMPLE_BUFFER_SIZE 50

class Storage {
public:
    Storage();
    void init(const Devices* d);
    void update();

    void printCardInfo();
    void printFiles();

    [[nodiscard]] uint8_t getNumSamples() const { return sampleBufferIndex + 1; }

    void refreshSampleRecordTimingMode();

private:
    void clearSampleBuffer();
    void recordSample();
    void writeBufferToDisk();
    bool shouldWriteSampleToDisk();
    bool updateLastWriteMarker();

    void startCard();
    void stopCard();

    DataSample sampleBuffer[SAMPLE_BUFFER_SIZE];
    unsigned char sampleBufferIndex;
    const Devices* devices;

    uint32_t lastExplorIRSampleNum = (uint32_t)-1;
    uint32_t lastRecordSeconds = (uint32_t)-1;
    StorageFileName storageFileName;

    RecordMode recordMode;
    uint32_t recordIntervalSeconds;
};


#endif //STALEMATE_STORAGE_H
