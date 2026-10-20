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

static void mcp2515_spi_timing_test(void)
{
    uint8_t canstat;

    printf("Starting MCP2515 SPI timing test\n");

    /* Select MCP2515 */
    select_slave(SLAVE_CAN);
    _delay_ms(1000);

    /* SPI command: READ */
    write_byte(MCP_READ);
    _delay_ms(1000);

    /* Register address: CANSTAT */
    write_byte(MCP_CANSTAT);
    _delay_ms(1000);

    /* Read the register value */
    canstat = read_byte();
    _delay_ms(1000);

    /* End the transaction */
    deselect_slaves();
    _delay_ms(1000);

    printf("CANSTAT read: 0x%02X\n", canstat);
}

int main(void)
{
    UART_Init(31);          /* CHECK: 31 = 9600 baud at 4.9152 MHz, keep your own value if different */
    sram_init();
    SPI_MasterInit();

    //mcp2515_spi_timing_test();

    timer_init();

    /* Register test: after reset CANSTAT must be 0x80 (configuration mode). */
    mcp2515_reset();
    printf("CANSTAT after reset: 0x%02X (expected 0x80)\n", mcp2515_read(MCP_CANSTAT));
    /*
     * CNF3 bits 4 and 3 are reserved and read as zero. Test only the
     * writable bits: SOF (bit 7), WAKFIL (bit 6), and PHSEG2 (bits 2:0).
     */
    mcp2515_write(MCP_CNF3, 0x45);
    printf("CNF3 wrote 0x45, read 0x%02X (expected 0x45)\n",
           mcp2515_read(MCP_CNF3));
    mcp2515_bit_modify(MCP_CNF3, 0x07, 0x02);
    printf("CNF3 after bit modify (expected 0x42): 0x%02X\n",
           mcp2515_read(MCP_CNF3));

    /* TODO: initialize CAN in loopback mode (can_init) */
    uint8_t result = can_init(CAN_MODE_LOOPBACK);
    if (result != 0) {
        printf("CAN initialization failed: %u\n", result);
        return 1;
    }

    mcp2515_interrupt_init();
    sei();

    can_message tx_message = {
        .id = 0x123,
        .length = 2,
        .data = {0xAA, 0x55}
    };
    can_message rx_message;
    uint8_t send_ticks = 0;

    while (1)
    {
        if (timer_tick()) {
            if (++send_ticks >= 50) {
                send_ticks = 0;

                if (!can_send(&tx_message)) {
                    printf("CAN send failed\n");
                }
            }
        }

        if (mcp2515_irq_flag) {
            mcp2515_irq_flag = 0;

            while (can_receive(&rx_message)) {
                printf("Received CAN message: ID=0x%03X length=%u data=",
                       rx_message.id, rx_message.length);

                for (uint8_t i = 0; i < rx_message.length; i++) {
                    printf("%02X ", rx_message.data[i]);
                }
                printf("\n");
            }
        }
    }
}
