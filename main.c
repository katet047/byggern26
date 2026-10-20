/* CAN / MCP2515 test (exercise 3.5).
 *
 * CHECK: this main does NOT call ui_init(). UI_main() has a while(1) loop and would
 *        block everything after it. To use the menu together with CAN, remove the
 *        UI_main() call from ui_init() and call it at the end of main instead.
 * CHECK: printf goes to the UART here. oled_init() (called from ui_init) redirects
 *        stdout to the OLED, so printf output would move to the screen.
 * CHECK: add mcp2515.c, can.c and timer.c to the sources in your Makefile.
 */

#define F_CPU 4915200UL

#include "UART.h"
#include "sram.h"
#include "SPI.h"
#include "can.h"
#include "mcp2515.h"
#include "timer.h"

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
    UART_Init(31);          /* CHECK: 31 = 9600 baud at 4.9152 MHz, keep your own value if different */
    sram_init();
    SPI_MasterInit();
    timer_init();

    /* Register test: after reset CANSTAT must be 0x80 (config mode) */
    mcp2515_reset();
    printf("CANSTAT after reset: 0x%02X (expected 0x80)\n", mcp2515_read(MCP_CANSTAT));
    mcp2515_write(MCP_CNF3, 0x55);
    printf("CNF3 wrote 0x55, read 0x%02X\n", mcp2515_read(MCP_CNF3));
    mcp2515_bit_modify(MCP_CNF3, 0x0F, 0x0A);
    printf("CNF3 after bit modify (expected 0x5A): 0x%02X\n", mcp2515_read(MCP_CNF3));

    /* TODO: initialize CAN in loopback mode (can_init) */

    sei();

    while (1)
    {
        /* TODO: every ~500 ms (use timer_tick(), 10 ms per tick), send a message with can_send() */

        /* TODO: when mcp2515_irq_flag is set, clear it and read messages with can_receive()
         *       until none is left (INT0 is edge triggered), then print them */
    }
}
