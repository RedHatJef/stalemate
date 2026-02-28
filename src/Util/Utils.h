//
// Created by redha on 3/10/2025.
//

#ifndef STALEMATE_UTILS_H
#define STALEMATE_UTILS_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define FRAMETIME_MILLIS (1000/60)

#define AVR_LEAVE_CRITICAL_REGION() SREG = saved_sreg;
#define AVR_ENTER_CRITICAL_REGION() uint8_t volatile saved_sreg = SREG; cli();

extern float getFeetFromMeters(float meters);
extern float getFahrenheitFromCelsius(float tempC);
extern void CCPWrite( volatile uint8_t * address, uint8_t value );
extern void setSystemClock(void);
extern int parseFiveDigits(const char* input);
extern void split_u32(uint32_t value, uint16_t *h, uint16_t *l);
extern void split_u32(uint32_t value, uint8_t* split4);
extern uint32_t combine_u32(uint16_t h, uint16_t l);
extern uint32_t combine_u32(const uint8_t* split4);

#endif //STALEMATE_UTILS_H
