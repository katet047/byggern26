#include "sam.h"
#include "pwm.h"


// Assumed here: servosig on PC19 (Due pin 44), peripheral B -> channel 5.
// NOT verified against our board: check the schematic and the SAM3X8E datasheet

#define PWM_CH 5  

void pwm_init(void)
{
    // Enable peripheral clocks for PIOC and the PWM module
    PMC->PMC_PCER0 = (1 << ID_PIOC);
    PMC->PMC_PCER1 = (1 << (ID_PWM - 32));


    PIOC->PIO_PDR  = PIO_PC19;
    PIOC->PIO_ABSR |= PIO_PC19;

    // CLKA = MCK / 84 = 1 MHz -> 1 tick = 1 us
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);

    // CPOL = 1: output starts high
    PWM->PWM_CH_NUM[PWM_CH].PWM_CMR  = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[PWM_CH].PWM_CPRD = 20000;          // 20 ms period
    PWM->PWM_CH_NUM[PWM_CH].PWM_CDTY = SERVO_MID_US;   

    // Enable the channel
    PWM->PWM_ENA = (1 << PWM_CH);
}

void servo_set_pulse_us(uint32_t us)
{
    if (us < SERVO_MIN_US) us = SERVO_MIN_US;   
    if (us > SERVO_MAX_US) us = SERVO_MAX_US;
    PWM->PWM_CH_NUM[PWM_CH].PWM_CDTYUPD = us;   
}

void servo_set_percent(uint8_t pct)
{
    if (pct > 100) pct = 100;
    // Map 0..100 % to 0.9..2.1 ms
    servo_set_pulse_us(SERVO_MIN_US + (SERVO_MAX_US - SERVO_MIN_US) * pct / 100);
}