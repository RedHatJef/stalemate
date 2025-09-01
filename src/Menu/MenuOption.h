//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_MENUOPTION_H
#define ETHAN1_MENUOPTION_H

#include "Devices.h"
#include "MenuKind.h"

class MenuOption {
public:
    explicit MenuOption(const char* title) : _title(title) {}
    virtual ~MenuOption() {}

    const char* getTitle() const { return _title; }
    virtual MenuKind kind() const = 0;
    virtual void setDevices(Devices* devices) { _devices = devices; }
    Devices* getDevices() const { return _devices; }

private:
    const char* _title;
    Devices* _devices;
};


#endif //ETHAN1_MENUOPTION_H
