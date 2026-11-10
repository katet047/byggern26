#pragma once
#include <stdint.h>

void solenoid_init(void);

// Call with the button state on every joystick message. Fires one pulse per new press.
void solenoid_button(uint8_t pressed);

// Call often from the main loop: ends the pulse when it has lasted long enough.
void solenoid_update(void);
