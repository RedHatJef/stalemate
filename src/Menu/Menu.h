//
// Created by redha on 9/1/2025.
//

#ifndef ETHAN1_MENU_H
#define ETHAN1_MENU_H

#include <Arduino.h>
#include "MenuOption.h"

static constexpr uint8_t MENU_MAX_ITEMS = 9;

class Menu : public MenuOption {
public:
    using MenuOption::MenuOption;

    bool addMenuOption(MenuOption* opt) {
        if (_count >= MENU_MAX_ITEMS) return false;
        _items[_count++] = opt;
        return true;
    }

    void setParent(Menu* p) { parent = p; }
    Menu* getParent() const { return parent; }

    void display() const {
        Serial.println();
        Serial.print(F("== ")); Serial.print(getTitle()); Serial.println(F(" =="));
        if (parent) {
            Serial.print(F("0: "));
            Serial.println(parent->getTitle());
        }
        for (uint8_t i = 0; i < _count; ++i) {
            Serial.print(i + 1);
            Serial.print(F(": "));
            Serial.println(_items[i]->getTitle());
        }
        Serial.print(F("\nCHOOSE> "));
    }

    MenuOption* getByIndex(uint8_t oneBased) const {
        // User enters 1..9; translate to 0..8
        if (oneBased == 0 || oneBased > _count) return nullptr;
        return _items[oneBased - 1];
    }

    uint8_t count() const { return _count; }

    MenuKind kind() const override { return MenuKind::Menu; }

    Menu* parent = nullptr;

private:
    MenuOption* _items[MENU_MAX_ITEMS] = { nullptr };
    uint8_t _count = 0;
};


#endif //ETHAN1_MENU_H
