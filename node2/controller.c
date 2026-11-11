// AI USE: This file was written with heavy use of AI (Claude), and then reviewed,
// edited and validated on the hardware by the candidates.

#include "sam.h"
#include "controller.h"
#include "encoder.h"
#include "motor.h"
#include "timer_counter.h"
#include "time.h"

#define CTRL_FREQ_HZ   100
#define CTRL_T         (1.0f / CTRL_FREQ_HZ)

// Gains work on position in percent of the track, so they don't depend on encoder resolution.
// u [% duty] = KP * e [%] + KI * integral(e) [%*s] - KD * velocity [%/s]
#define KP             3.0f
#define KI             2.0f
#define KD             0.15f           // damps overshoot, the driver lets the motor coast
#define I_LIMIT        (100.0f / KI)   // anti-windup: integral alone can at most saturate the output
#define DEADBAND       1.5f            // % error where we stop driving, avoids buzzing at target

// The motor needs some minimum duty before it moves at all (static friction). Below that,
// small joystick moves only creep along on the integral. Adding it as an offset makes small
// moves respond at once. Find it with USE_POSITION_CONTROL 0 in main.c: the smallest
// joystick deflection (= % duty) that just gets the racket moving.
#define U_FRICTION     15.0f           // % duty added in the direction of motion
// Caps the output so a big joystick move does not throw the racket across at full speed.
#define U_MAX          50.0f           // % duty, max 100

#define CALIB_SPEED    50              // % duty used to run into each end stop
#define CALIB_TIME_MS  1500

static int32_t range;                  // encoder counts from left end to right end (sign included)
static volatile float reference = 50.0f;
static float integral = 0.0f;
static float prev_position = 0.0f;

float controller_position(void)
{
    return encoder_read() * 100.0f / range;
}

void controller_set_reference(int8_t joystick)
{
    reference = (joystick + 100) / 2.0f;   // -100..100 -> 0..100 %
}

static void control_step(void)
{
    float position = controller_position();
    float e = reference - position;

    // D on the measurement instead of the error: same damping, but no kick when the
    // joystick reference jumps. Updated every step so it never sees a stale position.
    float velocity = (position - prev_position) / CTRL_T;
    prev_position = position;

    if (e > -DEADBAND && e < DEADBAND) {
        motor_set(0);
        return;
    }

    integral += e * CTRL_T;
    if (integral >  I_LIMIT) integral =  I_LIMIT;
    if (integral < -I_LIMIT) integral = -I_LIMIT;

    float u = KP * e + KI * integral - KD * velocity;

    // Friction compensation: start from the duty where the motor begins to move
    if (u > 0.0f) u += U_FRICTION;
    if (u < 0.0f) u -= U_FRICTION;

    if (u >  U_MAX) u =  U_MAX;
    if (u < -U_MAX) u = -U_MAX;

    motor_set((int16_t)u);
}

uint8_t controller_init(void)
{
    // Left end stop is position 0
    motor_set(-CALIB_SPEED);
    time_spinFor(msecs(CALIB_TIME_MS));
    motor_set(0);
    time_spinFor(msecs(200));
    encoder_reset();

    // Right end stop gives the range. Its sign absorbs any encoder/motor wiring mismatch,
    // so positive motor speed always increases controller_position().
    motor_set(CALIB_SPEED);
    time_spinFor(msecs(CALIB_TIME_MS));
    motor_set(0);
    time_spinFor(msecs(200));
    range = encoder_read();

    if (range > -100 && range < 100) {
        return 0;
    }

    reference = controller_position();   // hold current position until joystick input arrives
    prev_position = reference;           // so the first D step doesn't see a jump from 0
    timer_set_callback(control_step);
    timer_init(CTRL_FREQ_HZ);
    return 1;
}
