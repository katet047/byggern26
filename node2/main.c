#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#define F_CPU 84000000
#define BAUD 9600

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
        .reg = CAN_BR_PHASE2(4)
              | CAN_BR_PHASE1(5)
              | CAN_BR_PROPAG(1)
              | CAN_BR_SJW(0)
              | CAN_BR_BRP(5)
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

    static uint32_t diagnostic_counter = 0;

    while (1)
    { /* code */
        
        if (can_rx(&message)) {
            can_printmsg(message);
        }
        if (++diagnostic_counter >= 1000000) {
            diagnostic_counter = 0;
            //can_print_status();
            can_print_errormsg();
        }
       
    }
    
}