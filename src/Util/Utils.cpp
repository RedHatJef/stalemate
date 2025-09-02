//
// Created by redha on 3/10/2025.
//

#include <new>
#include <string.h>
#include "Utils.h"

float getFeetFromMeters(float meters) {
    double cm = meters * 100;
    double inches = cm / 2.54;
    double feet = inches / 12.0;
    return feet;
}

float getFahrenheitFromCelsius(float tempC)
{
    return (tempC * 9.0 / 5.0) + 32.0;
}

void CCPWrite( volatile uint8_t * address, uint8_t value )
{
    AVR_ENTER_CRITICAL_REGION();
    volatile uint8_t * tmpAddr = address;
#ifdef RAMPZ
    RAMPZ = 0;
#endif
    asm volatile(
            "movw r30,  %0"	      "\n\t"
            "ldi  r16,  %2"	      "\n\t"
            "out   %3, r16"	      "\n\t"
            "st     Z,  %1"       "\n\t"
            :
            : "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (&CCP)
            : "r16", "r30", "r31"
            );

    AVR_LEAVE_CRITICAL_REGION();
}

///* This function initializes the CLKCTRL module */
void setSystemClock(void)
{
    CCPWrite((uint8_t*)&(CLKCTRL.OSCHFCTRLA), (CLKCTRL.OSCHFCTRLA | CLKCTRL_FREQSEL_24M_gc));
}

int parseFiveDigits(const char* charDigits) {
    char input[6];
    memset(input, 0, sizeof(input));

    // Verify all five characters are digits
    for (int i = 0; i < 5; i++) {
        char c = charDigits[i];
        if (c < '0' || c > '9') {
            return -1; // invalid character
        }
        input[i] = c;
    }

    // Verify null terminator
    if (input[5] != '\0') {
        return -1; // not properly null-terminated
    }

    // Use atoi to convert
    int value = atoi(input);
    return value;
}

