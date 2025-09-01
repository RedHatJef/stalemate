//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETYEAR_H
#define STALEMATE_SETYEAR_H

#include "DateTimeSetter.h"

class SetYear : public DateTimeSetter {
public:
    SetYear() : DateTimeSetter(F("Set Year (2025..2099)")) {}
    void onValue(long value) override;
};

#endif //STALEMATE_SETYEAR_H
