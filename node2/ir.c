#include "sam.h"
#include "ir.h"
#include "time.h"


static uint8_t  channel = 0;


static uint16_t samples[IR_FILTER_LEN];
static uint32_t sum = 0;
static uint8_t  idx = 0;


static uint16_t filtered = 0;
static uint8_t  blocked = 0;
static uint16_t score = 0;
static uint64_t last_sample_time = 0;
static uint64_t last_goal_time = 0;

void ir_init(uint8_t adc_channel)
{
    channel = adc_channel;

    PMC->PMC_PCER1 = (1 << (ID_ADC - 32));

    // ADC clock = MCK / ((PRESCAL + 1) * 2) = 84 MHz / 8 = 10.5 MHz (max is 20 MHz)
    ADC->ADC_MR = ADC_MR_PRESCAL(3)
                | ADC_MR_STARTUP_SUT64
                | ADC_MR_TRACKTIM(15)
                | ADC_MR_TRANSFER(2)
                | ADC_MR_FREERUN_ON;


    ADC->ADC_CHER = (1 << channel);
    ADC->ADC_CR   = ADC_CR_START;

    while (!(ADC->ADC_ISR & (1u << channel))) {}

    uint16_t first = ir_read_raw();
    for (uint8_t i = 0; i < IR_FILTER_LEN; i++) samples[i] = first;
    sum = (uint32_t)first * IR_FILTER_LEN;
    filtered = first;
    blocked = 0;
    last_sample_time = time_now();
    last_goal_time = time_now();
}

uint16_t ir_read_raw(void)
{
    return (uint16_t)(ADC->ADC_CDR[channel] & 0xFFF);
}

void ir_update(void)
{
    // Take at most one sample every IR_SAMPLE_PERIOD_MS
    uint64_t now = time_now();
    if (now - last_sample_time < msecs(IR_SAMPLE_PERIOD_MS)) return;
    last_sample_time = now;

    //replace the oldest sample with the new one
    uint16_t s = ir_read_raw();
    sum -= samples[idx];
    samples[idx] = s;
    sum += s;
    idx = (idx + 1) % IR_FILTER_LEN;
    filtered = (uint16_t)(sum / IR_FILTER_LEN);


    if (!blocked && filtered < IR_THRESH_BLOCKED) {
        blocked = 1;

        if (now - last_goal_time >= msecs(IR_GOAL_LOCKOUT_MS)) {
            score++;
            last_goal_time = now;
        }
    } else if (blocked && filtered > IR_THRESH_FREE) {
        blocked = 0;
    }
}

uint16_t ir_read_filtered(void) { return filtered; }
uint8_t  ir_beam_blocked(void)  { return blocked; }
uint16_t score_get(void)        { return score; }
void     score_reset(void)      { score = 0; }
