#pragma once
#include <stdint.h>

void motor_init(void);
// Speed in percent, -100..100. Sign selects direction (PHASE pin).
void motor_set(int16_t speed);
