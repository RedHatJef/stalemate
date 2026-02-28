//
// Created by redha on 9/20/2025.
//

#include "EEPROMData.h"
#include "Util/Utils.h"
#include <EEPROM.h>

void EEPROMData::setRecordMode(RecordMode r) {
    EEPROM.write(0, (uint8_t)r);

    dirty = true;
}

RecordMode EEPROMData::getRecordMode() {
    return (RecordMode)EEPROM.read(0);
}

void EEPROMData::setRecordIntervalSeconds(uint32_t seconds) {
    uint8_t splitSeconds[4];
    split_u32(seconds, splitSeconds);

    EEPROM.update(1, splitSeconds[0]);
    EEPROM.update(2, splitSeconds[1]);
    EEPROM.update(3, splitSeconds[2]);
    EEPROM.update(4, splitSeconds[3]);

    dirty = true;
}

uint32_t EEPROMData::getRecordIntervalSeconds() {
    uint8_t splitSeconds[4];

    splitSeconds[0] = EEPROM.read(1);
    splitSeconds[1] = EEPROM.read(2);
    splitSeconds[2] = EEPROM.read(3);
    splitSeconds[3] = EEPROM.read(4);

    return combine_u32(splitSeconds);
}

const char *EEPROMData::getRecordModeString() {
    RecordMode r = getRecordMode();
    uint32_t intervalSeconds = getRecordIntervalSeconds();
    return getRecordModeString(r, intervalSeconds);
}

const char *EEPROMData::getRecordModeString(RecordMode r, uint32_t i) {
    switch(r) {
        case RECORDMODE_EVERY_UPDATE:
            snprintf(printBuf, sizeof(printBuf), "Data Record Mode (%u) --> Save Every ExplorIR Update (~2x per second)", r);
            break;
        case RECORDMODE_SECONDS:
            snprintf(printBuf, sizeof(printBuf), "Data Record Mode (%u) --> Save Every (%u) seconds", r, i);
            break;
        default:
            snprintf(printBuf, sizeof(printBuf), "Data Record Mode (%u,%u) --> ???", r, i);
            break;
    }

    return printBuf;
}
