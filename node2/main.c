#include <stdio.h>
#include <stdarg.h>
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
// 1 = exercise 8.4: joystick x sets racket position, PI controller drives the motor
#define USE_POSITION_CONTROL 1
#define JOY_DEADZONE 5

// 1 = print the IR reading twice a second, for tuning the thresholds in ir.h.
// Set to 0 when playing: printf blocks ~1 ms per character at 9600 baud, and
// while it blocks ir_update() is not called, so a fast ball can be missed.
#define PRINT_IR_DEBUG 1

/*
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */
//#include "../path_to/uart.h"

static void can_print_status(void)
{
    uint32_t status = CAN0->CAN_SR;
    uint32_t errors = CAN0->CAN_ECR;

    printf("CAN_BR=0x%08lX CAN_SR=0x%08lX CAN_ECR=0x%08lX\n\r",
           (unsigned long)CAN0->CAN_BR,
           (unsigned long)status,
           (unsigned long)errors);
}

static void can_print_errormsg(){
    uint32_t ecr = CAN0->CAN_ECR;

    printf("CAN_BR=0x%08lX CAN_SR=0x%08lX CAN_ECR=0x%08lX "
       "REC=%lu TEC=%lu\n\r",
       (unsigned long)CAN0->CAN_BR,
       (unsigned long)CAN0->CAN_SR,
       (unsigned long)ecr,
       (unsigned long)(ecr & CAN_ECR_REC_Msk),
       (unsigned long)((ecr & CAN_ECR_TEC_Msk) >> CAN_ECR_TEC_Pos));
}

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    //Uncomment after including uart above
    uart_init(F_CPU, BAUD);
    //printf("Hello World\n\r");

    CanInit can_timing = {
        /* 125 kbit/s, same as Node 1 (MCP2515, CNF1 = 0x03).
         * CAN clock = MCK = 84 MHz, tq = (BRP+1)/84 MHz = 42/84 MHz = 500 ns
         * 1 sync + 2 prop + 7 PS1 + 6 PS2 = 16 TQ = 8 us, sample point 62.5% */
        .reg = CAN_BR_PHASE2(5)     /* 6 TQ */
              | CAN_BR_PHASE1(6)    /* 7 TQ */
              | CAN_BR_PROPAG(1)    /* 2 TQ */
              | CAN_BR_SJW(0)       /* 1 TQ */
              | CAN_BR_BRP(41)
              | CAN_BR_SMP_ONCE
    };
    can_init(can_timing, 0);
    printf("CAN initialized\n\r");
    printf("CAN_BR=0x%08lX\n\r", (unsigned long)CAN0->CAN_BR);
    printf("CAN_MR=0x%08lX CAN_SR=0x%08lX CAN_BR=0x%08lX\n\r",
       (unsigned long)CAN0->CAN_MR,
       (unsigned long)CAN0->CAN_SR,
       (unsigned long)CAN0->CAN_BR);

    CanMsg message;

    ir_init(7); // REVIEW: assumed A0 = PA16 = AD7, verify against the shield schematic
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

    uint16_t sent_lives = 0xFFFF;       // never a real value, so the first loop sends the lives
#if PRINT_IR_DEBUG
    uint64_t last_ir_print = 0;
#endif

    while (1)
    { /* code */
        
        ir_update();
        solenoid_update();

        // Each miss (ball lost into the IR beam) costs a life. Sent to Node 1 whenever it
        // changes; if the mailbox is still busy it is retried next loop, so this never blocks.
        uint16_t misses = ir_misses();
        uint8_t  lives = (misses >= GAME_LIVES) ? 0 : GAME_LIVES - misses;
        if (lives != sent_lives && can_tx_ready()) {
            sent_lives = lives;
            printf(lives ? "Lives=%u\n\r" : "Game over\n\r", lives);
            can_tx((CanMsg){
                .id = CAN_ID_LIVES,
                .length = 1,
                .byte = { lives },
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
                // All lives back. A ball still resting in the beam is not a new miss:
                // the beam has to clear and be blocked again before it counts.
                ir_reset_misses();
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