#include "sam.h"
#include "pwm.h"

// Servo signal: SAM3X PB13 = Arduino Due D21 (SCL), peripheral B = PWMH1 -> PWM channel 1.
// Source: TTK4155 Motor Shield doc (v26), SERVO header pin 2 (SIG).

#define PWM_CH 1

void pwm_init(void)
{
    // Enable peripheral clocks for PIOB and the PWM module
    PMC->PMC_PCER0 = (1 << ID_PIOB);
    PMC->PMC_PCER1 = (1 << (ID_PWM - 32));

    // Hand PB13 to the PWM peripheral (B)
    PIOB->PIO_PDR  = PIO_PB13;
    PIOB->PIO_ABSR |= PIO_PB13;

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

// AI USE: the deadzone/hysteresis in servo_set_signed() (servo jitter fix) was written with
// heavy use of AI (Claude), and then reviewed, edited and validated by the candidates.
// The joystick ADC flickers about +-1 LSB (~1.2 %, ~7 us), which makes the servo twitch.
#define SERVO_DEADZONE    5   // % around joystick center treated as exactly center
#define SERVO_HYSTERESIS  3   // % change needed before the pulse width is updated

void servo_set_signed(int pct)
{
    static int last = 0;

    if (pct < -100) pct = -100;
    if (pct >  100) pct =  100;
    if (pct > -SERVO_DEADZONE && pct < SERVO_DEADZONE) pct = 0;

    // Ignore small changes, but always let center and the end points through
    int diff = pct - last;
    if (diff > -SERVO_HYSTERESIS && diff < SERVO_HYSTERESIS
        && pct != 0 && pct != 100 && pct != -100) {
        return;
    }
    last = pct;

    servo_set_pulse_us(SERVO_MID_US + (SERVO_MAX_US - SERVO_MIN_US) / 2 * pct / 100);
}