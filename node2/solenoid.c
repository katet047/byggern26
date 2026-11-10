#include "sam.h"
#include "solenoid.h"
#include "time.h"

// Relay driver (lab manual fig. 28): pin -> 1k -> MPSA56 base (PNP, emitter on 3.3 V),
// collector -> relay coil -> GND. The relay switches 12 V to the solenoid.
// Active low: pin low = transistor on = relay pulls in = solenoid fires.
// REVIEW: D9 = SAM3X PC21 assumed, change both lines if the resistor goes to another pin.
#define SOLENOID_PIO          PIOC
#define SOLENOID_PIN          PIO_PC21

#define SOLENOID_PULSE_MS     50    // just long enough to hit the ball; tune on the board
#define SOLENOID_COOLDOWN_MS  400   // minimum time between shots, lets the solenoid retract

static uint8_t  firing = 0;
static uint8_t  last_button = 0;
static uint64_t fire_time = 0;

void solenoid_init(void)
{
    PMC->PMC_PCER0 = (1 << ID_PIOC);

    // Drive high (off) before enabling the output, so it never glitches low and fires at startup
    SOLENOID_PIO->PIO_SODR = SOLENOID_PIN;
    SOLENOID_PIO->PIO_PER  = SOLENOID_PIN;
    SOLENOID_PIO->PIO_OER  = SOLENOID_PIN;

    // Pretend the last shot was a full cooldown ago, so the first press fires right away
    fire_time = time_now() - msecs(SOLENOID_COOLDOWN_MS);
}

void solenoid_button(uint8_t pressed)
{
    // Only a 0 -> 1 change is a new press; holding the button fires once
    uint8_t new_press = pressed && !last_button;
    last_button = pressed;

    if (!new_press || firing) return;
    if (time_now() - fire_time < msecs(SOLENOID_COOLDOWN_MS)) return;

    SOLENOID_PIO->PIO_CODR = SOLENOID_PIN;   // low = fire
    fire_time = time_now();
    firing = 1;
}

void solenoid_update(void)
{
    // Non-blocking: the pulse always ends by itself, even if CAN messages stop
    if (firing && time_now() - fire_time >= msecs(SOLENOID_PULSE_MS)) {
        SOLENOID_PIO->PIO_SODR = SOLENOID_PIN;   // high = off
        firing = 0;
    }
}
