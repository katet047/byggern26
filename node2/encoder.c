#include "sam.h"
#include "encoder.h"

// Motor encoder: SAM3X PC25 = Due D5 (TIOA6, PHA), PC26 = Due D4 (TIOB6, PHB), peripheral B.
// TC2 channel 0 (= TC6) in quadrature decoder mode, counting edges on both phases (4x).

void encoder_init(void)
{
    // Enable peripheral clocks for PIOC and TC2 channel 0 (TC6)
    PMC->PMC_PCER0 = (1 << ID_PIOC);
    PMC->PMC_PCER1 = (1 << (ID_TC6 - 32));

    // Hand PC25/PC26 to TC2 (peripheral B)
    PIOC->PIO_PDR   = PIO_PC25B_TIOA6 | PIO_PC26B_TIOB6;
    PIOC->PIO_ABSR |= PIO_PC25B_TIOA6 | PIO_PC26B_TIOB6;

    // Quadrature decoder, position mode
    TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN;

    // Channel 0 counts the decoded position, clocked by XC0
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

// Counter is 32 bits and counts down past 0, so read it as signed
int32_t encoder_read(void)
{
    return (int32_t)TC2->TC_CHANNEL[0].TC_CV;
}

void encoder_reset(void)
{
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
}
