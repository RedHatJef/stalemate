//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_MENUOPTION_H
#define STALEMATE_MENUOPTION_H

#include "Devices.h"
#include "MenuKind.h"

class MenuOption {
public:
    explicit MenuOption(const __FlashStringHelper* title) : _title(title) {}
    virtual ~MenuOption() {}

    const __FlashStringHelper* getTitle() const { return _title; }
    virtual MenuKind kind() const = 0;
    virtual void setDevices(Devices* devices) { _devices = devices; }
    Devices* getDevices() const { return _devices; }

private:
    const __FlashStringHelper* _title;
    Devices* _devices;
};


#endif //STALEMATE_MENUOPTION_H
