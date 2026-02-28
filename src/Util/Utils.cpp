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

void split_u32(uint32_t value, uint16_t *h, uint16_t *l) {
    *l = (uint16_t)(value & 0xFFFF);       // lower 16 bits
    *h = (uint16_t)((value >> 16) & 0xFFFF); // upper 16 bits
}

void split_u32(uint32_t value, uint8_t *split4) {
    split4[3] = value & 0xFF;
    value >>= 8;
    split4[2] = value & 0xFF;
    value >>= 8;
    split4[1] = value & 0xFF;
    value >>= 8;
    split4[0] = value & 0xFF;
}

uint32_t combine_u32(const uint8_t *split4) {
    uint32_t v = split4[0];
    v <<= 8;
    v += split4[1];
    v <<= 8;
    v += split4[2];
    v <<= 8;
    v += split4[3];
    return v;
}

extern uint32_t combine_u32(uint16_t h, uint16_t l) {
    uint32_t v = h;
    v = v << 16;
    v += l;
    return v;
}



