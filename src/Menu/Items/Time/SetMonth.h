//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETMONTH_H
#define STALEMATE_SETMONTH_H

#include "DateTimeSetter.h"

class SetMonth : public DateTimeSetter {
public:
    SetMonth() : DateTimeSetter("Set Month (1..12)") { }
    void onValue(long value) override;
};

#endif //STALEMATE_SETMONTH_H
