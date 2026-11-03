#pragma once
#include <stdint.h>


void timer_init(uint32_t freq_hz);
uint32_t timer_ticks(void);
void timer_set_callback(void (*cb)(void));
