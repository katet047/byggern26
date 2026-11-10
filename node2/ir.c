#include "sam.h"
#include "ir.h"

// IR photodiode (via op-amp, 0..3.3 V) on Arduino Due A0 = SAM3X PA16 = ADC channel 7.
// Note the Due A-pin numbers run opposite to the ADC channels: A0 = AD7, A1 = AD6, ..., A7 = AD0.
#define IR_ADC_CH   7

// Hysteresis thresholds in raw ADC counts. Assumes blocked beam = low voltage.
// Tune these from the printed ir_filtered() values with the beam clear and blocked.
#define IR_BLOCKED_BELOW  1000
#define IR_CLEAR_ABOVE    1500

#define IR_AVG_LEN  4   // samples in the moving average (4 x 10 ms)

static uint16_t samples[IR_AVG_LEN];
static uint8_t  sample_idx = 0;
static uint32_t sample_sum = 0;
static uint8_t  blocked = 0;

void ir_init(void)
{
    PMC->PMC_PCER1 = (1 << (ID_ADC - 32));

    // Enabling the channel takes the pin over as analog input, no PIO setup needed.
    // ADC clock = MCK / (2 * (PRESCAL + 1)) = 84 MHz / 42 = 2 MHz (max 22 MHz).
    // Free run: the ADC converts continuously, CDR always holds the newest sample.
    ADC->ADC_MR = ADC_MR_FREERUN_ON
                | ADC_MR_PRESCAL(20)
                | ADC_MR_STARTUP_SUT64;
    ADC->ADC_CHER = (1 << IR_ADC_CH);
    ADC->ADC_CR   = ADC_CR_START;

    // Start the average at the first reading instead of 0, so we don't report a false goal
    for (volatile uint32_t i = 0; i < 1000; i++);
    uint16_t first = ir_read_raw();
    for (uint8_t i = 0; i < IR_AVG_LEN; i++) samples[i] = first;
    sample_sum = (uint32_t)first * IR_AVG_LEN;
    blocked = first < IR_BLOCKED_BELOW;
}

uint16_t ir_read_raw(void)
{
    return ADC->ADC_CDR[IR_ADC_CH] & ADC_CDR_DATA_Msk;
}

uint16_t ir_filtered(void)
{
    return sample_sum / IR_AVG_LEN;
}

uint8_t ir_update(void)
{
    uint16_t raw = ir_read_raw();
    sample_sum += raw - samples[sample_idx];
    samples[sample_idx] = raw;
    sample_idx = (sample_idx + 1) % IR_AVG_LEN;

    uint16_t value = ir_filtered();

    // Two thresholds so noise around a single level can't toggle the state
    if (!blocked && value < IR_BLOCKED_BELOW) {
        blocked = 1;
        return 1;
    }
    if (blocked && value > IR_CLEAR_ABOVE) {
        blocked = 0;
    }
    return 0;
}
