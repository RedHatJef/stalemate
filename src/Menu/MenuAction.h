//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_MENUACTION_H
#define STALEMATE_MENUACTION_H

#include "MenuOption.h"
#include "Devices.h"

class MenuAction : public MenuOption {
public:
    using MenuOption::MenuOption;
    virtual void doAction() = 0;
    MenuKind kind() const override { return MenuKind::Action; }
};

#endif //STALEMATE_MENUACTION_H
