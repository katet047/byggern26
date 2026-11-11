#pragma once
#include <stdint.h>

#define SERVO_MIN_US 900
#define SERVO_MAX_US 2100
#define SERVO_MID_US 1500

void pwm_init(void);

// Pulse width in us, always clamped to SERVO_MIN_US..SERVO_MAX_US
void servo_set_pulse_us(uint32_t us);

// Joystick position -100..100 % -> servo, with a deadzone and hysteresis against jitter
void servo_set_signed(int pct);
