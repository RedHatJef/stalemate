//
// Created by redha on 3/10/2025.
//

#include "Storage.h"
#include "SdFat.h"

#undef STORAGE_DEBUGGING
#undef STORAGE_VERBOSE_DEBUGGING

#define STORAGE_PRINT(x)                    Serial.print(x)
#define STORAGE_PRINTLN(x)                  Serial.println(x)
#define STORAGE_PRINTF1(x, y)               Serial.printf(x, y)
#define STORAGE_PRINTF2(x, y, z)            Serial.printf(x, y, z)
#define STORAGE_PRINTF3(x, y, z, w)         Serial.printf(x, y, z, w)

#ifdef STORAGE_VERBOSE_DEBUGGING
#define STORAGE_VERBOSELN(x)                Serial.println(x)
#define STORAGE_VERBOSE(x)                  Serial.print(x)
#define STORAGE_VERBOSE_PRINTF1(x, y)       Serial.printf(x, y)
#define STORAGE_VERBOSE_PRINTF2(x, y, z)    Serial.printf(x, y, z)
#define STORAGE_VERBOSE_PRINTF3(x, y, z, w) Serial.printf(x, y, z, w)
#else
#define STORAGE_VERBOSELN(x)
#define STORAGE_VERBOSE(x)
#define STORAGE_VERBOSE_PRINTF1(x, y)
#define STORAGE_VERBOSE_PRINTF2(x, y, z)
#define STORAGE_VERBOSE_PRINTF3(x, y, z, w)
#endif

#ifdef STORAGE_DEBUGGING
#define STORAGE_DEBUGLN(x)                  Serial.println(x)
#define STORAGE_DEBUG(x)                    Serial.print(x)
#define STORAGE_DEBUG_PRINTF1(x, y)         Serial.printf(x, y)
#define STORAGE_DEBUG_PRINTF2(x, y, z)      Serial.printf(x, y, z)
#define STORAGE_DEBUG_PRINTF3(x, y, z, w)   Serial.printf(x, y, z, w)
#else
#define STORAGE_DEBUGLN(x)
#define STORAGE_DEBUG(x)
#define STORAGE_DEBUG_PRINTF1(x, y)
#define STORAGE_DEBUG_PRINTF2(x, y, z)
#define STORAGE_DEBUG_PRINTF3(x, y, z, w)
#endif

// chip select pin
static const int chipSelect = 7;

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
//#define SPI_CLOCK SD_SCK_MHZ(50)
#define SPI_CLOCK SD_SCK_MHZ(32)

#define SD_CONFIG SdSpiConfig(SS, DEDICATED_SPI, SPI_CLOCK)

static SdFat32 sd;

static const char* designators[] = { "b\0", "kb\0", "mb\0", "gb\0", "tb\0", "pb\0", "eb\0" };
static const char* getPrintableFileSize(char* buf, uint64_t fileSize)
{
    uint8_t designatorIndex = 0;
    while(fileSize > 1024) {
        designatorIndex++;
        fileSize /= 1024;
    }

    const char* designator = designators[designatorIndex];
    sprintf(buf, "%u", fileSize, designator);
    strcat(buf, designator);
    return buf;
}

Storage::Storage() {
    lastWrite = 0;
    sampleBufferIndex = 0;
    clearSampleBuffer();
}

void Storage::clearSampleBuffer() {
    for(int i = 0; i < SAMPLE_BUFFER_SIZE; i++) {
        sampleBuffer[i].clear();
    }
    sampleBufferIndex = 0;
}

void Storage::writeBufferToDisk() {
    STORAGE_DEBUG_PRINTF1(F("Storage: Writing buffer with (%d) samples to disk\r\n"), sampleBufferIndex+1);

    startCard();

    if(storageFileName.hasNewFileName()) {
        storageFileName.updateFileName();
        STORAGE_DEBUG_PRINTF1(F("Storage: Updated file name to (%s)\r\n"), storageFileName.getCurrentFileName());
    }

    const char* filename = storageFileName.getCurrentFileName();
    char dataBuf[512];
    File32 file;

    if(sd.exists(filename)) {
        STORAGE_DEBUG_PRINTF2(F("Storage: Appending file (%s) with (%d) samples\r\n"), filename, sampleBufferIndex+1);
        file = sd.open(filename, O_WRONLY | O_APPEND);
        if(file.getWriteError()) {
            STORAGE_PRINTLN(F("Storage: Failed to open file for appending.\r\n"));
            clearSampleBuffer();
            return;
        }
    }
    else
    {
        STORAGE_DEBUG_PRINTF2(F("Storage: Creating new file (%s) with (%d) samples\r\n"), filename, sampleBufferIndex+1);
        file = sd.open(filename, O_WRONLY | O_CREAT);
        if(file.getWriteError()) {
            STORAGE_PRINTLN(F("Storage: Failed to open file for writing.\r\n"));
            clearSampleBuffer();
            return;
        }

        file.write(DataSample::tableHeader);
        if(file.getWriteError()) {
            STORAGE_PRINTLN(F("Storage: Failed to write header to file.\r\n"));
            clearSampleBuffer();
            return;
        }
    }

    for(int i = 0; i < sampleBufferIndex; i++) {
        const char* csvString = sampleBuffer[i].getDataString(dataBuf, sizeof(dataBuf));
        file.write(csvString);

        if(file.getWriteError()) {
            STORAGE_PRINTLN(F("Storage: ERROR writing data to file.\r\n"));
            file.clearWriteError();
            break;
        }
    }
    file.close();

    STORAGE_DEBUG_PRINTF1(F("Storage: Wrote (%d) samples to disk\r\n"), sampleBufferIndex+1);

    stopCard();

    clearSampleBuffer();
}

void Storage::init(const Devices *d) {
    devices = d;

    startCard();
    stopCard();

    storageFileName.init(d);

    STORAGE_DEBUGLN(F("Storage: Started."));
}

void Storage::startCard()
{
    STORAGE_DEBUGLN(F("Storage: Starting Card"));

    while(1) {
        if(!sd.begin(SD_CONFIG))
        {
            STORAGE_PRINTLN(F("Storage: Failed to initialize card, retrying."));
            delay(10);
            sd.end();
            delay(500);
        }
        else
        {
            break;
        }
    }

    if(sd.fatType() != FAT_TYPE_FAT32) {
        STORAGE_PRINTF1(F("Storage: Incorrect fat type detected (%s)"), sd.fatType());
        while(1) delay(10);
    }
}

void Storage::stopCard()
{
    STORAGE_DEBUGLN(F("Storage: Stopping Card"));
    sd.end();
}

static void waitForCard() {
    if(sd.isBusy()) {
        STORAGE_VERBOSE(F("Storage: Waiting for SD card to finish."));
        while(sd.isBusy()) {
            STORAGE_VERBOSE('.');
            delay(1);
        }
        STORAGE_VERBOSELN("");
    }
}

void Storage::recordSample() {
    if(sampleBufferIndex >= SAMPLE_BUFFER_SIZE) {
        writeBufferToDisk();
        clearSampleBuffer();
    }

    STORAGE_VERBOSE("Storage: Recording sample, ExplorIR Sample Num = ");
    STORAGE_VERBOSELN(devices->explorIR->getNumSamples());
    sampleBuffer[sampleBufferIndex].setFromDevices(devices);
    sampleBufferIndex++;
}

void Storage::update()
{
//    if(millis() - lastWrite < WRITE_INTERVAL_SECONDS * 1000) return;
    unsigned long currentSampleNum = devices->explorIR->getNumSamples();
    if(lastExplorIRSampleNum == currentSampleNum) return;
    lastExplorIRSampleNum = currentSampleNum;
    recordSample();
}

void Storage::printFiles() {
    startCard();
    waitForCard();
    sd.ls(LS_R | LS_DATE | LS_SIZE);
    stopCard();
}

void Storage::printCardInfo() {
    startCard();
    waitForCard();

    if(sd.vol()->fatType() == 0) {
        STORAGE_PRINTLN(F("Storage: No valid FAT16/FAT32/exFAT partition."));
        return;
    }

    uint32_t size = sd.card()->sectorCount();
    if (size == 0) {
        STORAGE_PRINTLN(F("Storage: Can't determine the card size."));
        return;
    }

    uint32_t sizeMB = 0.000512 * size + 0.5;
    STORAGE_PRINT(F("Card size: "));
    STORAGE_PRINT(sizeMB);
    STORAGE_PRINTLN(F(" MB (MB = 1,000,000 bytes)"));
    if (sd.fatType() <= 32) {
        STORAGE_PRINT(F("Volume is FAT"));
        STORAGE_PRINT(int(sd.fatType()));
    } else {
        STORAGE_PRINT(F("Volume is exFAT"));
    }
    STORAGE_PRINT(F(", Cluster size (bytes): "));
    STORAGE_PRINTLN(sd.vol()->bytesPerCluster());

    if ((sizeMB > 1100 && sd.vol()->sectorsPerCluster() < 64) ||
        (sizeMB < 2200 && sd.vol()->fatType() == 32)) {
        STORAGE_PRINTLN(F("\nThis card should be reformatted for best performance.\n"));
        STORAGE_PRINTLN(F("   Use a cluster size of 32 KB for cards larger than 1 GB."));
        STORAGE_PRINTLN(F("   Only cards larger than 2 GB should be formatted FAT32."));
        return;
    }
    STORAGE_PRINTLN("");
    stopCard();
}



