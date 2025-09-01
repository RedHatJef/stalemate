//
// Created by redha on 3/10/2025.
//

#include "Storage.h"
#include "SdFat.h"

#ifdef STORAGE_ENABLE_DEBUGGING
#define STORAGE_DEBUG(x) Serial.println(x)
#else
#define STORAGE_DEBUG(x)
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
    STORAGE_DEBUG("Storage: Clearing sample buffer")
    memset(sampleBuffer, 0, sizeof(sampleBuffer));
    sampleBufferIndex = 0;
}

void Storage::writeBufferToDisk() {
    STORAGE_DEBUG("Storage: Writing buffer to disk")
}

void Storage::init(const Devices *d) {
    devices = d;

    if(!sd.begin(SD_CONFIG)) {
        Serial.println(F("SDCard: Failed to initialize card."));
        while(1) delay(10);
    }
    else if(sd.fatType() != FAT_TYPE_FAT32) {
        Serial.printf(F("SDCard: Incorrect fat type detected (%s)"), sd.fatType());
        while(1) delay(10);
    }

    STORAGE_DEBUG(F("SDCard: Started."));
}

static void waitForCard() {
    if(sd.isBusy()) {
#if SDCARD_VERBOSE
        Serial.print(F("Waiting for SD card to finish."));
#endif
        while(sd.isBusy()) {
#if SDCARD_VERBOSE
            Serial.print('.');
#endif
            delay(1);
        }
#if SDCARD_VERBOSE
        Serial.println();
#endif
    }
}

void Storage::recordSample() {
    return;
    if(sampleBufferIndex >= SAMPLE_BUFFER_SIZE) {
        writeBufferToDisk();
        clearSampleBuffer();
    }

    STORAGE_DEBUG("Storage: Recording sample.")
    DataSample* data = &sampleBuffer[sampleBufferIndex];
    memcpy(data->date, devices->clock->getDateString(), sizeof(data->date));
    memcpy(data->time, devices->clock->getTimeString(), sizeof(data->time));
//    data->bmp390TempC = devices->bmp->getTempC();
//    data->sht31TempC = devices->sht->getTempC();
//    data->sht31HumidityPercent = devices->sht->getHumidity();
//    data->co2PPM = devices->scd40->getCO2PPM();
    sampleBufferIndex++;
}

void Storage::update() {
    return;
    if(millis() - lastWrite > WRITE_INTERVAL_SECONDS * 1000)
    {
        recordSample();
    }
}

void Storage::printFiles() {
    waitForCard();
    sd.ls(LS_R | LS_DATE | LS_SIZE);
}

void Storage::printCardInfo() {
    waitForCard();

    if(sd.vol()->fatType() == 0) {
        Serial.println(F("No valid FAT16/FAT32/exFAT partition."));
        return;
    }

    uint32_t size = sd.card()->sectorCount();
    if (size == 0) {
        Serial.println(F("Can't determine the card size."));
        return;
    }

    uint32_t sizeMB = 0.000512 * size + 0.5;
    Serial.print(F("Card size: "));
    Serial.print(sizeMB);
    Serial.println(F(" MB (MB = 1,000,000 bytes)"));
    if (sd.fatType() <= 32) {
        Serial.print(F("Volume is FAT"));
        Serial.print(int(sd.fatType()));
    } else {
        Serial.print(F("Volume is exFAT"));
    }
    Serial.print(F(", Cluster size (bytes): "));
    Serial.println(sd.vol()->bytesPerCluster());

    if ((sizeMB > 1100 && sd.vol()->sectorsPerCluster() < 64) ||
        (sizeMB < 2200 && sd.vol()->fatType() == 32)) {
        Serial.println(F("\nThis card should be reformatted for best performance.\n"));
        Serial.println(F("   Use a cluster size of 32 KB for cards larger than 1 GB."));
        Serial.println(F("   Only cards larger than 2 GB should be formatted FAT32."));
        return;
    }
    Serial.println();
}

