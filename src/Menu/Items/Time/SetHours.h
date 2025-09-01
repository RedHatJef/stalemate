//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETHOURS_H
#define STALEMATE_SETHOURS_H

#include "DateTimeSetter.h"

class SetHours : public DateTimeSetter {
public:
    SetHours() : DateTimeSetter(F("Set Hour (0..23)")) {}
    void onValue(long value) override;
};

#endif //STALEMATE_SETHOURS_H
