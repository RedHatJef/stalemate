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

    uint8_t getNumSamples() const { return sampleBufferIndex + 1; }

private:
    void clearSampleBuffer();
    void recordSample();
    void writeBufferToDisk();

    void startCard();
    void stopCard();

    const unsigned int WRITE_INTERVAL_SECONDS = 1;
    DataSample sampleBuffer[SAMPLE_BUFFER_SIZE];
    unsigned char sampleBufferIndex;
    const Devices* devices;
    unsigned long lastWrite;

    unsigned long lastExplorIRSampleNum = (unsigned long)-1;
    StorageFileName storageFileName;
};


#endif //STALEMATE_STORAGE_H
