//
// Created by redha on 9/14/2025.
//

#ifndef STALEMATE_STORAGEFILENAME_H
#define STALEMATE_STORAGEFILENAME_H

#include "Devices.h"

class StorageFileName {
public:
    StorageFileName() { }

    void init(const Devices* d) {
        devices = d;
        updateFileName();
    }

    const char* getCurrentFileName() const
    {
        return filename;
    }

    void updateFileName() {
        memset(filename, 0, sizeof(filename));

        const Clock* c = devices->clock;
        year = c->getYear();
        month = c->getMonth();
        day = c->getDay();
        hour = c->getHour();
        minute = c->getMinute();

        snprintf(filename, sizeof(filename), "%04d%02d%02d_%02d.csv",
                 year, month, day,
                 hour
                 );
    }

    bool hasNewFileName() const {
        const Clock* c = devices->clock;
        //if(c->getMinute() != minute) return true;
        if(c->getHour() != hour) return true;
        if(c->getDay() != day) return true;
        if(c->getMonth() != month) return true;
        if(c->getYear() != year) return true;
        return false;
    }

private:
    const Devices* devices;
    char filename[64];
    uint16_t year = (uint16_t)-1;
    uint8_t month = (uint8_t)-1;
    uint8_t day = (uint8_t)-1;
    uint8_t hour = (uint8_t)-1;
    uint8_t minute = (uint8_t)-1;
};

#endif //STALEMATE_STORAGEFILENAME_H
