#pragma once
#include <stdint.h>

#define SERVO_MIN_US 900
#define SERVO_MAX_US 2100
#define SERVO_MID_US 1500

void pwm_init(void);
void servo_set_signed(int16_t value);