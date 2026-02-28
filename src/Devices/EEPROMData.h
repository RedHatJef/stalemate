//
// Created by redha on 9/19/2025.
//

#ifndef STALEMATE_EEPROMDATA_H
#define STALEMATE_EEPROMDATA_H

#include <Arduino.h>
#include "RecordMode.h"

class EEPROMData {
public:
    EEPROMData() { }

    void setRecordMode(RecordMode r);
    [[nodiscard]] RecordMode getRecordMode();

    void setRecordIntervalSeconds(uint32_t seconds);
    [[nodiscard]] uint32_t getRecordIntervalSeconds();

    [[nodiscard]] const char* getRecordModeString();
    [[nodiscard]] const char* getRecordModeString(RecordMode r, uint32_t i);

    [[nodiscard]] bool isDirty() const { return dirty; }
    void clearDirty() { dirty = false; }

private:
    char printBuf[100];
    bool dirty = false;
};

#endif //STALEMATE_EEPROMDATA_H
