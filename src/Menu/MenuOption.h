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

private:
    const char* _title;
};


#endif //ETHAN1_MENUOPTION_H
