//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_STORAGE_H
#define STALEMATE_STORAGE_H

#include "Devices.h"
#include "DataSample.h"

#define SAMPLE_BUFFER_SIZE 5

class Storage {
public:
    Storage();
    void init(const Devices* d);
    void update();

    void printCardInfo();
    void printFiles();

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


#endif //STALEMATE_STORAGE_H
