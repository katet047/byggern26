#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#include "joystick.h"
#include "pwm.h"
#include "motor.h"
#include "encoder.h"
#include "controller.h"
#define F_CPU 84000000
#define BAUD 9600

// 0 = exercise 8.3: joystick x sets motor speed directly (open loop)
// 1 = exercise 8.4: joystick x sets racket position, PI controller drives the motor
#define USE_POSITION_CONTROL 1
#define JOY_DEADZONE 5

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
    
    pwm_init();
    motor_init();
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

    static uint32_t diagnostic_counter = 0;

    while (1)
    { /* code */
        
        if (can_rx(&message)) {
            Joystick joy;
            if (joystick_from_can(&message, &joy)) {
                // Node 1 sends at 50 Hz; print every 10th (5 Hz) so the 9600 baud UART keeps up
                servo_set_signed(joy.y);

                if (closed_loop) {
                    controller_set_reference(joy.x);
                } else {
                    int8_t speed = (joy.x > -JOY_DEADZONE && joy.x < JOY_DEADZONE) ? 0 : joy.x;
                    motor_set(speed);
                }
                
                static uint8_t print_count = 0;
                if (++print_count >= 10) {
                    print_count = 0;
                    static const char *dir_names[] = {"NEUTRAL", "LEFT", "RIGHT", "UP", "DOWN"};
                    printf("Joystick x=%4d%% y=%4d%% dir=%-7s btn=%u enc=%ld\n\r",
                           joy.x, joy.y, dir_names[joy.dir], joy.button,
                           (long)encoder_read());
                }
            } else {
                can_printmsg(message);
            }
        }
        if (++diagnostic_counter >= 1000000) {
            diagnostic_counter = 0;
            //can_print_status();
            can_print_errormsg();
        }
       
    }
    
}