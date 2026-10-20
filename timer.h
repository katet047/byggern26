#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/* 100 Hz (10 ms) tick using Timer3 in CTC mode, to pace SPI/CAN traffic (bus contention) */
void timer_init(void);
uint8_t timer_tick(void);   /* returns 1 if a tick is pending (and consumes it) */

#endif
