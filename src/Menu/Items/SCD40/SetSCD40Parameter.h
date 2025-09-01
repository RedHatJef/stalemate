//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_SETSCD40PARAMETER_H
#define STALEMATE_SETSCD40PARAMETER_H

#include "Menu/MenuActionInput.h"
#include "Menu/MenuUtils.h"

class SetSCD40Parameter : public MenuActionInput {
public:
    using MenuActionInput::MenuActionInput;

    void doAction() override {
        // Enter input capture for this instance
        EnterInputMode(this);
    }
};

#endif //STALEMATE_SETSCD40PARAMETER_H
