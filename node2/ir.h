#pragma once
#include <stdint.h>

// REVIEW: the thresholds below are NOT measured, they are placeholders.
// our own analog circuit, so they must be calibrated on the real board:
//   1. Read "IR=<value>" from the terminal with the beam free, and note it.
//   2. Read it again with the beam blocked, and note it.
//   3. Set the thresholds between the two readings (raw ADC counts, 0..4095),
//      always with IR_THRESH_BLOCKED < IR_THRESH_FREE (the gap is the hysteresis).
// If the signal goes UP when the beam is blocked, swap the comparisons in ir_update().

#define IR_THRESH_BLOCKED  1500   // REVIEW: filtered value below this -> beam blocked
#define IR_THRESH_FREE     2500   // REVIEW: filtered value above this -> beam free again

#define IR_SAMPLE_PERIOD_MS  1    // one ADC sample every 1 ms
#define IR_FILTER_LEN       16    
#define IR_GOAL_LOCKOUT_MS 500    // ignore new goals for 500 ms after a goal


void ir_init(uint8_t adc_channel);

uint16_t ir_read_raw(void);


void ir_update(void);


uint16_t ir_read_filtered(void);
uint8_t  ir_beam_blocked(void);


uint16_t score_get(void);
void     score_reset(void);
