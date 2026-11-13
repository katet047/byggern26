// AI USE: This file was written with heavy use of AI (Claude), and then reviewed,
// edited and validated on the hardware by the candidates.

#pragma once
#include <stdint.h>

void motor_init(void);
// Speed in percent, -100..100. Sign selects direction (PHASE pin).
void motor_set(int16_t speed);
