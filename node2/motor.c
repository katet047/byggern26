#include "sam.h"
#include "motor.h"

// REVIEW: pins are ASSUMED, verify against the motor shield schematic.
//   PHASE (direction): PC23 (Due pin 7), plain GPIO
//   ENABLE (speed): PC24 (Due pin 6), peripheral B = PWM channel 7
#define MOTOR_PHASE_PIN   PIO_PC23
#define MOTOR_PWM_PIN     PIO_PC24
#define MOTOR_PWM_CH      7

// REVIEW: set to 1 if the motor turns the wrong way for positive joystick values
#define MOTOR_DIR_INVERT  0


#define MOTOR_DEADZONE_PCT  5
#define MOTOR_MAX_PCT       100
// CLKB = MCK / 4 = 21 MHz, period 1050 ticks -> 20 kHz PWM 
#define MOTOR_PWM_PERIOD  1050

void motor_init(void)
{
    PMC->PMC_PCER0 = (1 << ID_PIOC);
    PMC->PMC_PCER1 = (1 << (ID_PWM - 32));

    // output, start low
    PIOC->PIO_PER  = MOTOR_PHASE_PIN;
    PIOC->PIO_OER  = MOTOR_PHASE_PIN;
    PIOC->PIO_CODR = MOTOR_PHASE_PIN;

    PIOC->PIO_PDR   = MOTOR_PWM_PIN;
    PIOC->PIO_ABSR |= MOTOR_PWM_PIN;

    PWM->PWM_CLK |= PWM_CLK_PREB(0) | PWM_CLK_DIVB(4);


    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CMR  = PWM_CMR_CPRE_CLKB | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CPRD = MOTOR_PWM_PERIOD;
    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CDTY = 0;      

    PWM->PWM_ENA = (1 << MOTOR_PWM_CH);
}

void motor_set_signed(int pct)
{
    if (pct < -100) pct = -100;
    if (pct >  100) pct =  100;

    int forward = (pct >= 0);
    int mag = forward ? pct : -pct;

    if (mag < MOTOR_DEADZONE_PCT) mag = 0;
    if (mag > MOTOR_MAX_PCT)      mag = MOTOR_MAX_PCT;

    if (forward ^ MOTOR_DIR_INVERT) {
        PIOC->PIO_SODR = MOTOR_PHASE_PIN;
    } else {
        PIOC->PIO_CODR = MOTOR_PHASE_PIN;
    }

    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CDTYUPD = (uint32_t)mag * MOTOR_PWM_PERIOD / 100;
}

