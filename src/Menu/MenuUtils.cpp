//
// Created by redha on 9/1/2025.
//

#include "MenuUtils.h"
#include "MenuActionInput.h"
#include "UiState.h"
#include "Menu/Items/Main/MainMenu.h"

static MainMenu gMainMenu;
static Menu* gCurrentMenu = &gMainMenu;

static UiState gUiState = UiState::Idle;

// When ReadingInt, this points to the active action awaiting a value
MenuActionInput* gPendingInput = nullptr;

// Simple input buffer for integer parsing
static constexpr uint8_t INBUF_SIZE = 16;
char gInbuf[INBUF_SIZE];
uint8_t gInlen = 0;

void setMenuDevices(Devices* devices) {
    gMainMenu.setDevices(devices);
}

void setMenuStorage(Storage* s) {
    gMainMenu.setStorage(s);
}

// ======== Small helper for input actions to enter input mode (no RTTI) ========
void EnterInputMode(MenuActionInput* action) {
    beginIntegerInput(action);
}

void beginIntegerInput(MenuActionInput* action) {
    gPendingInput = action;
    gUiState = UiState::ReadingInt;
    resetInputBuffer();
    gPendingInput->startInput();
}

void resetInputBuffer() {
    gInlen = 0;
    gInbuf[0] = '\0';
}

void finishIntegerInput() {
    // Parse long
    gInbuf[gInlen] = '\0';
    char* endptr = nullptr;
    long val = strtol(gInbuf, &endptr, 10);

    if (gInlen == 0 || endptr == gInbuf) {
        Serial.println(F("\nInvalid integer. (No value entered)"));
    } else if (*endptr != '\0') {
        Serial.println(F("\nInvalid integer. (Unexpected characters)"));
    } else {
        Serial.println();
        gPendingInput->submitValue(val);
    }

    gPendingInput = nullptr;
    gUiState = UiState::Idle;
    gCurrentMenu->display();
}

static void handleIntegerChar(int c) {
    if (c == '\r' || c == '\n') {
        finishIntegerInput();
        return;
    }

    // Backspace (ASCII 8 or 127)
    if (c == 8 || c == 127) {
        if (gInlen > 0) {
            gInlen--;
            gInbuf[gInlen] = '\0';
            // Visual backspace on terminal:
            Serial.print((char)8);  // move back
            Serial.print(' ');      // erase
            Serial.print((char)8);  // move back again
        }
        return;
    }

    // Allow digits, and a single leading +/-
    bool ok = false;
    if (gInlen < (INBUF_SIZE - 1)) {
        if (c >= '0' && c <= '9') ok = true;
        if (gInlen == 0 && (c == '-' || c == '+')) ok = true;
    }

    if (ok) {
        gInbuf[gInlen++] = (char)c;
        gInbuf[gInlen] = '\0';
        Serial.print((char)c); // echo
    } else {
        // Ignore other characters while in integer input
    }
}

static void handleMenuChoice(uint8_t digit) {
    if (digit == 0) {
        if (gCurrentMenu->getParent()) {
            gCurrentMenu = gCurrentMenu->getParent();
        } else {
            Serial.println(F("(Already at top-level)"));
        }
        gCurrentMenu->display();
        return;
    }

    MenuOption* opt = gCurrentMenu->getByIndex(digit);
    if (!opt) {
        Serial.println(F("Invalid choice."));
        gCurrentMenu->display();
        return;
    }

    if (opt->kind() == MenuKind::Menu) {
        gCurrentMenu = static_cast<Menu*>(opt);
        gCurrentMenu->display();
    } else {
        // Action chosen
        // If it’s an input-capable action, begin input mode.
        // We avoid RTTI by trying a safe downcast pattern you control;
        // here we rely on a helper: try calling doAction and let it start input.
        // To enable input, we downcast only if the action *is* MenuActionInput.
        // Since we don’t use RTTI, we provide a helper.
        MenuAction* act = static_cast<MenuAction*>(opt);

        // Try to detect input-capable via a small interface trick:
        // We add a virtual "isInputAction()" only in MenuActionInput via hidden override?
        // Simpler: attempt a known pointer if created as MenuActionInput before.
        // Here we'll call doAction(). If it’s a MenuActionInput, it will call startInput(),
        // and we arrange beginIntegerInput() from outside by using a custom hook:
        // To keep it explicit and simple, we’ll introduce a tiny convention:
        // Input actions will call a global function to enter input mode.

        Serial.print(static_cast<char>(digit + '0'));
        Serial.print(" = ");
        Serial.println(act->getTitle());
        Serial.println();
        act->doAction();

        // After doAction(), if it was a normal action, re-display menu.
        if (gUiState == UiState::Idle) {
            gCurrentMenu->display();
        }
    }
}

void displayMenu() {
    gCurrentMenu->display();
}

void processSerialInput() {
    if (Serial.available() > 0) {
        int c = Serial.read();

        if (gUiState == UiState::ReadingInt) {
            handleIntegerChar(c);
            return;
        }

        // Normal menu navigation
        if (c >= '0' && c <= '9') {
            uint8_t digit = static_cast<uint8_t>(c - '0');
            handleMenuChoice(digit);
        } else if (c == '\n' || c == '\r') {
            // ignore
        } else {
            Serial.println(F("Please enter a digit 0-9."));
        }
    }
}