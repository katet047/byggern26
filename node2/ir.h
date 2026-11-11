#pragma once
#include <stdint.h>

// The thresholds are relative to the clear-beam level ("baseline"). The baseline is measured
// in ir_init()/ir_calibrate() and then slowly follows the signal while the beam is clear,
// so drift from room light, LED aim or temperature does not break detection.
//
// REVIEW: set the drops from the terminal ("IR=<filtered> base=<baseline>"):
//   1. Note IR with the beam clear and with the beam blocked. drop = clear - blocked.
//   2. IR_DROP_BLOCKED ~ 50 % of that drop, IR_DROP_CLEAR ~ 25 % (the gap is the hysteresis).
// If the signal goes UP when the beam is blocked, swap the comparisons in ir_update().
#define IR_DROP_BLOCKED     80    // REVIEW: filtered this far below baseline -> beam blocked
#define IR_DROP_CLEAR       40    // REVIEW: back within this of baseline -> beam clear again
#define IR_MIN_BASELINE     100   // clear level below this: beam not reaching the diode, no misses counted

#define IR_SAMPLE_PERIOD_MS 1     // one ADC sample every 1 ms
#define IR_FILTER_LEN       20    // 20 ms average: cancels 50/100 Hz light flicker, fast enough for the ball
#define IR_BASELINE_SHIFT   12    // baseline tracking time constant = 2^12 samples * 1 ms ~ 4 s
#define IR_CALIB_MS         200   // averaging time when measuring the baseline
#define IR_MISS_LOCKOUT_MS  500   // ignore new misses for 500 ms after a miss


void ir_init(uint8_t adc_channel);

// Measure the clear-beam level again. The beam must be clear while this runs (~200 ms).
void ir_calibrate(void);

uint16_t ir_read_raw(void);


void ir_update(void);


uint16_t ir_read_filtered(void);
uint16_t ir_baseline(void);
uint8_t  ir_beam_blocked(void);
uint8_t  ir_ok(void);             // 0 if the clear level is too low to detect anything


// Number of times the beam went from clear to blocked (ball lost). A ball resting in
// the beam counts once; it must be taken out (beam clear) before the next miss counts.
uint16_t ir_misses(void);
void     ir_reset_misses(void);




/* the issue is mabye that the IR beam registering is too strickt as to what it thinks is a blocked beam. it registers a hand right in front of the beam ass blocked ,but only sometimes the ball */