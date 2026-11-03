#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

/* CHECK: assumes F_CPU = 4915200 Hz -> 4915200 / 1024 = 4800 Hz, OCR3A = 47 -> 4800 / 48 = 100 Hz */
static volatile uint8_t ticks;

void timer_init(void){
    TCCR3A = 0;
    TCCR3B = (1 << WGM32) | (1 << CS32) | (1 << CS30);   /* CTC mode, prescaler 1024 */
    OCR3A  = 47;
    TCNT3  = 0;
    ETIMSK |= (1 << OCIE3A);                             /* compare A interrupt enable */
}

uint8_t timer_tick(void){
    uint8_t t = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        if (ticks){ ticks--; t = 1; }
    }
    return t;
}

ISR(TIMER3_COMPA_vect){
    if (ticks < 255) ticks++;
}
