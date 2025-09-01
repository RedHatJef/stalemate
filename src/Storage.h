//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_STORAGE_H
#define ETHAN1_STORAGE_H

#include "Devices.h"
#include "DataSample.h"

#define SAMPLE_BUFFER_SIZE 5

class Storage {
public:
    Storage();
    void init(const Devices* d);
    void update();

private:
    void clearSampleBuffer();
    void recordSample();
    void writeBufferToDisk();

    const unsigned int WRITE_INTERVAL_SECONDS = 1;
    DataSample sampleBuffer[SAMPLE_BUFFER_SIZE];
    unsigned char sampleBufferIndex;
    const Devices* devices;
    unsigned long lastWrite;
};


#endif //ETHAN1_STORAGE_H
