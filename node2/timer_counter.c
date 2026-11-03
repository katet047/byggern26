#include "sam.h"
#include "timer_counter.h"

static volatile uint32_t ticks = 0;
static void (*callback)(void) = 0;

void timer_set_callback(void (*cb)(void)) { callback = cb; }

void timer_init(uint32_t freq_hz)
{
    PMC->PMC_PCER0 = (1 << ID_TC0);

    // TIMER_CLOCK4 = MCK/128 = 656.25 kHz
    TC0->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK4
                              | TC_CMR_WAVE
                              | TC_CMR_WAVSEL_UP_RC;
    TC0->TC_CHANNEL[0].TC_RC  = (84000000UL / 128) / freq_hz;
    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;

    NVIC_EnableIRQ(TC0_IRQn);
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void TC0_Handler(void)
{
    (void)TC0->TC_CHANNEL[0].TC_SR;   
    ticks++;
    if (callback) callback();
}

uint32_t timer_ticks(void) { return ticks; }
