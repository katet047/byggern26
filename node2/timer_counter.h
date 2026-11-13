#pragma once
#include <stdint.h>

// TC0 channel 0 interrupt at freq_hz, calls the callback from the interrupt
void timer_init(uint32_t freq_hz);
void timer_set_callback(void (*cb)(void));
