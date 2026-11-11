#include "sam.h"
#include "ir.h"

static uint8_t channel = 0;

void ir_init(uint8_t adc_channel)
{
    channel = adc_channel;

    PMC->PMC_PCER1 = (1 << (ID_ADC - 32));   // enable the ADC clock (ADC = ID 37)
    ADC->ADC_CR = ADC_CR_SWRST;              // reset the ADC

    // Software-triggered, 10-bit, same settings as the reference solution.
    // ADC clock = MCK / ((PRESCAL + 1) * 2) = 84 MHz / 6 = 14 MHz (max is 20 MHz)
    ADC->ADC_MR = ADC_MR_TRGEN_DIS
                | ADC_MR_LOWRES_BITS_10
                | ADC_MR_PRESCAL(2)
                | ADC_MR_STARTUP_SUT8
                | ADC_MR_SETTLING_AST9
                | ADC_MR_TRACKTIM(15)
                | ADC_MR_TRANSFER(2);

    ADC->ADC_CHER = (1 << channel);
    ADC->ADC_IDR  = 0xFFFFFFFF;              // no interrupts, we poll
}

uint16_t ir_read_raw(void)
{
    ADC->ADC_CR = ADC_CR_START;              // start one conversion

    for (uint32_t i = 0; !(ADC->ADC_ISR & (1u << channel)); i++) {
        if (i > 10000) return IR_TIMEOUT;    // conversion never finished
    }
    return (uint16_t)(ADC->ADC_CDR[channel] & 0x3FF);
}

uint8_t ir_beam_blocked(void)
{
    uint16_t value = ir_read_raw();
    return value != IR_TIMEOUT && value < IR_THRESHOLD;
}
