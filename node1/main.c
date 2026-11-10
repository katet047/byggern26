/* CAN / MCP2515 test (exercise 3.5).
 *
 * The menu runs non-blocking: ui_init() draws it, ui_update() is called every
 * 10 ms tick. printf goes to the UART; the menu draws on the OLED via &mystdout.
 * CHECK: add mcp2515.c, can.c and timer.c to the sources in your Makefile.
 */

#define F_CPU 4915200UL

#include "UART.h"
#include "sram.h"
#include "SPI.h"
#include "can.h"
#include "mcp2515.h"
#include "timer.h"
#include "IO.h"
#include "joystick_can.h"
#include "UI.h"
#include "game.h"

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
    IO_init();              /* ADC clock on PD5 + joystick button input on PB1 */

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

    /* Initialize the CAN controller for communication on the physical bus. */
    uint8_t result = can_init(CAN_MODE_NORMAL);
    if (result != 0) {
        printf("CAN initialization failed: %u\n", result);
        return 1;
    }

    mcp2515_interrupt_init();
    printf("CNF1=0x%02X CNF2=0x%02X CNF3=0x%02X\n",
       mcp2515_read(MCP_CNF1),
       mcp2515_read(MCP_CNF2),
       mcp2515_read(MCP_CNF3));

    sei();

    /* Joystick must be at rest here: this sets the 0 % center position */
    _delay_ms(100);
    calibrate();
    printf("Joystick calibrated\n");

    ui_init();              /* draws the menu on the OLED; printf stays on the UART */

    can_message rx_message;
    uint8_t send_ticks = 0;

    while (1)
    {
        if (timer_tick()) {
            /* 100 Hz: menu navigation (joystick + buttons) */
            ui_update();
            game_update();      /* sends "start game" to Node 2 when the TX buffer is free */

            /* 100 Hz tick -> send joystick every 2nd tick = 50 Hz */
            if (++send_ticks >= 2) {
                send_ticks = 0;

                if (!joystick_can_send()) {
                    /* TX buffer still busy: usually means no ACK (is Node 2 connected?) */
                    static uint8_t fail_count = 0;
                    if (++fail_count >= 50) {   /* report at most once per second */
                        fail_count = 0;
                        printf("CAN send failed: TXB0CTRL=0x%02X EFLG=0x%02X\n",
                               mcp2515_read(MCP_TXB0CTRL),
                               mcp2515_read(MCP_EFLG));
                    }
                }
            }
        }

        if (mcp2515_irq_flag) {
            mcp2515_irq_flag = 0;

            while (can_receive(&rx_message)) {
                game_handle_can(&rx_message);

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
