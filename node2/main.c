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

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    //Uncomment after including uart above
    uart_init(F_CPU, BAUD);
    //printf("Hello World\n\r");

    CanInit can_timing = {
        .phase2 = 1,
        .propag = 1,
        .phase1 = 2,
        .sjw = 0,
        .brp = 20,
        .smp = 0
    };
    can_init(can_timing, 0);
    printf("CAN initialized\n\r");
    CanMsg message;

    while (1)
    { /* code */
        
        if (can_rx(&message)) {
            can_printmsg(message);
        }
       
    }
    
}