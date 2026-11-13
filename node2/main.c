// AI USE: the IR, motor/controller, solenoid and game (misses over CAN) parts of this file
// were written with heavy use of AI (Claude), and then reviewed, edited and validated on the
// hardware by the candidates. The final CAN bit timing below was written by AI (see comment).

#include <stdio.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#include "joystick.h"
#include "ir.h"
#include "time.h"
#include "pwm.h"
#include "motor.h"
#include "encoder.h"
#include "controller.h"
#include "solenoid.h"
#include "game.h"
#define F_CPU 84000000
#define BAUD 9600

// 0 = exercise 8.3: joystick x sets motor speed directly (open loop)
// 1 = exercise 8.4: joystick x sets racket position, PID controller drives the motor
#define USE_POSITION_CONTROL 1
#define JOY_DEADZONE 5

// 1 = print the IR reading twice a second, for tuning the thresholds in ir.h.
// Keep 0 when playing: printf blocks ~1 ms per character at 9600 baud, and
// while it blocks ir_update() is not called, so a fast ball can be missed.
#define PRINT_IR_DEBUG 0

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; // Disable watchdog timer

    uart_init(F_CPU, BAUD);

    CanInit can_timing = {
        /* 125 kbit/s, same bit time and sample point as Node 1 (MCP2515, see node1/mcp2515.c).
         * The first version was made by the candidates but did not work; after debugging with
         * AI (Claude) the fault was found, and the final timing was written by AI and validated
         * by the candidates.
         *
         *   N          = Sync + Prop + PS1 + PS2 = 1 + 2 + 7 + 6 = 16 TQ per bit
         *   tq         = (BRP + 1) / MCK               (SAM3X datasheet 40.7.4.1, no factor 2)
         *   BRP        = MCK / (N * bitrate) - 1 = 84 MHz / (16 * 125 kHz) - 1 = 41
         *   tq         = 42 / 84 MHz = 500 ns
         *   bit time   = N * tq = 16 * 500 ns = 8 us  ->  125 kbit/s
         *   sample pt  = (Sync + Prop + PS1) / N = 10 / 16 = 62.5 %
         *
         * CAN_BR stores each length - 1 (CAN_BR = 0x00290165). Datasheet limits: 8-25 TQ per
         * bit, PHASE2 >= 2 TQ and <= PHASE1, BRP 1..127. */
        .reg = CAN_BR_PHASE2(5)     /* 6 TQ */
              | CAN_BR_PHASE1(6)    /* 7 TQ */
              | CAN_BR_PROPAG(1)    /* 2 TQ */
              | CAN_BR_SJW(0)       /* 1 TQ */
              | CAN_BR_BRP(41)
              | CAN_BR_SMP_ONCE
    };
    can_init(can_timing, 0);
    printf("CAN initialized\n\r");

    CanMsg message;
    
    pwm_init(); 

    ir_init(7); // A0 = PA16 = ADC channel 7
    pwm_init();
    motor_init();
    solenoid_init();
    encoder_init();

#if USE_POSITION_CONTROL
    printf("Calibrating motor range...\n\r");
    uint8_t closed_loop = controller_init();
    if (!closed_loop) {
        printf("Calibration failed: no encoder movement, falling back to open loop\n\r");
    }
#else
    uint8_t closed_loop = 0;
#endif

    uint32_t sent_misses = 0xFFFFFFFF;  // never a real value, so the first loop sends the misses
#if PRINT_IR_DEBUG
    uint64_t last_ir_print = 0;
#endif

    while (1)
    {
        ir_update();
        solenoid_update();

        // Each miss (ball lost into the IR beam) is sent to Node 1, which turns it into
        // lives for the game mode. Sent whenever it changes; if the mailbox is still busy
        // it is retried next loop, so this never blocks.
        uint16_t misses = ir_misses();
        if (misses != sent_misses && can_tx_ready()) {
            sent_misses = misses;
            printf("Misses=%u\n\r", misses);
            can_tx((CanMsg){
                .id = CAN_ID_MISSES,
                .length = 2,
                .byte = { misses & 0xFF, misses >> 8 },
            });
        }

#if PRINT_IR_DEBUG
        if (time_now() - last_ir_print >= msecs(500)) {
            last_ir_print = time_now();
            printf("IR=%4u base=%4u blocked=%u%s\n\r", ir_read_filtered(), ir_baseline(),
                   ir_beam_blocked(), ir_ok() ? "" : " (NO BEAM: check LED aim)");
        }
#endif

        if (can_rx(&message)) {
            Joystick joy;
            if (message.id == CAN_ID_GAME_START) {
                // Misses back to 0. A ball still resting in the beam is not a new miss:
                // the beam has to clear and be blocked again before it counts.
                // Always send the 0, even if it was 0 already: Node 1 waits for it to know
                // that everything it receives after this belongs to the new game.
                ir_reset_misses();
                sent_misses = 0xFFFFFFFF;
                printf("New game\n\r");
            } else if (joystick_from_can(&message, &joy)) {
                servo_set_signed(joy.y);
                solenoid_button(joy.shoot);

                if (closed_loop) {
                    controller_set_reference(joy.x);
                } else {
                    int8_t speed = (joy.x > -JOY_DEADZONE && joy.x < JOY_DEADZONE) ? 0 : joy.x;
                    motor_set(speed);
                }
            } else {
                can_printmsg(message);
            }
        }
    }
}
