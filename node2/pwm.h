#pragma once
#include <stdint.h>

#define SERVO_MIN_US 900
#define SERVO_MAX_US 2100
#define SERVO_MID_US 1500


void pwm_init(void);
void servo_set_pulse_us(uint32_t us);
void servo_set_percent(uint8_t pct);
