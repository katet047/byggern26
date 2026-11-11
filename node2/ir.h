#pragma once
#include <stdint.h>

// IR beam detection, done the same way as the TTK4155-main reference solution:
// one raw 10-bit ADC sample per tick, compared with a fixed threshold.
// No filter, no baseline and no hysteresis.
//
// REVIEW: check IR_THRESHOLD on the terminal (PRINT_IR_DEBUG in main.c).
// It must lie between the raw value with the beam blocked and with the beam clear.
// If the value goes UP when the beam is blocked, flip the comparison in ir_beam_blocked().
#define IR_THRESHOLD        100   // raw value (0..1023) below this -> beam blocked
#define IR_SAMPLE_PERIOD_MS 8     // sample every ~8 ms (~120 Hz, like the reference)

void     ir_init(uint8_t adc_channel);

// Start one conversion and wait for it. Returns 0..1023, or IR_TIMEOUT on timeout.
uint16_t ir_read_raw(void);
#define IR_TIMEOUT 0xFFFF

// Takes a new sample and returns 1 if the beam is blocked.
uint8_t  ir_beam_blocked(void);
