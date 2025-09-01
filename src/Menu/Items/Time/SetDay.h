//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETDAY_H
#define STALEMATE_SETDAY_H


#include "DateTimeSetter.h"

class SetDay : public DateTimeSetter {
public:
    SetDay() : DateTimeSetter("Set Day of Month (1..31)") { }
    void onValue(long value) override;
};


#endif //STALEMATE_SETDAY_H
