//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETSECONDS_H
#define STALEMATE_SETSECONDS_H

#include "DateTimeSetter.h"

class SetSeconds : public DateTimeSetter {
public:
    SetSeconds() : DateTimeSetter(F("Set Second (0..59)")) {}
    void onValue(long value) override;
};

#endif //STALEMATE_SETSECONDS_H
