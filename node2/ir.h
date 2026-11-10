#pragma once
#include <stdint.h>

void ir_init(void);

// Latest raw 12-bit ADC sample (0..4095 = 0..3.3 V)
uint16_t ir_read_raw(void);

// Moving average of the last samples, as used for detection
uint16_t ir_filtered(void);

// Call at a fixed rate (every 10 ms). Returns 1 once when the beam goes from clear to blocked.
uint8_t ir_update(void);
