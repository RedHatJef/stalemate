//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETMINUTES_H
#define STALEMATE_SETMINUTES_H

#include "DateTimeSetter.h"

class SetMinutes : public DateTimeSetter {
public:
    SetMinutes() : DateTimeSetter(F("Set Minute (0..59)")) { }
    void onValue(long value) override;
};

#endif //STALEMATE_SETMINUTES_H
