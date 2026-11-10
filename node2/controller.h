#pragma once
#include <stdint.h>

// Calibrates the encoder range by driving the racket to both ends, then starts
// the PI position controller on a TC0 interrupt. Returns 0 if calibration failed
// (no encoder movement seen), in which case the controller is not started.
uint8_t controller_init(void);

// Target position from joystick, -100 (left end) .. 100 (right end)
void controller_set_reference(int8_t joystick);

// Current position in percent of the track, 0..100
float controller_position(void);
