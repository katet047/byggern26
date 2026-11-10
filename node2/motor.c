#include "sam.h"
#include "motor.h"

// A3959 in phase/enable mode (TTK4155 Motor Shield doc):
//   ENABLE: SAM3X PB12 = Due D20, peripheral B = PWMH0 -> PWM channel 0 (speed)
//   PHASE:  SAM3X PC23 = Due D7, GPIO output (direction)

#define MOTOR_PWM_CH  0
#define MOTOR_PERIOD  4200   // MCK / 4200 = 20 kHz, above audible range
#define MOTOR_PHASE   PIO_PC23
//#define MOTOR_DEADZONE 10

void motor_init(void)
{
    // Enable peripheral clocks for PIOB, PIOC and the PWM module
    PMC->PMC_PCER0 = (1 << ID_PIOB) | (1 << ID_PIOC);
    PMC->PMC_PCER1 = (1 << (ID_PWM - 32));

    // Hand PB12 to the PWM peripheral (B)
    PIOB->PIO_PDR   = PIO_PB12B_PWMH0;
    PIOB->PIO_ABSR |= PIO_PB12B_PWMH0;

    // PHASE as GPIO output, start low
    PIOC->PIO_PER  = MOTOR_PHASE;
    PIOC->PIO_OER  = MOTOR_PHASE;
    PIOC->PIO_CODR = MOTOR_PHASE;

    // Clocked straight from MCK so the servo's CLKA setup is untouched.
    // CPOL = 1: CDTY is the high (enabled) time, same as the servo channel.
    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CMR  = PWM_CMR_CPRE_MCK | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CPRD = MOTOR_PERIOD;
    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CDTY = 0;   // motor off

    PWM->PWM_ENA = (1 << MOTOR_PWM_CH);
}

void motor_set(int16_t speed)
{
    if (speed < -100) speed = -100;
    if (speed >  100) speed =  100;

    if (speed < 0) {
        PIOC->PIO_SODR = MOTOR_PHASE;
        speed = -speed;
    } else {
        PIOC->PIO_CODR = MOTOR_PHASE;
    }

    PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CDTYUPD = (uint32_t)speed * MOTOR_PERIOD / 100;
}
