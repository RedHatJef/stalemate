//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_UTILS_H
#define ETHAN1_UTILS_H

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

#endif //ETHAN1_UTILS_H
