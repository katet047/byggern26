/* Node 1: joystick, buttons, OLED menu and game screen.
 *
 * The menu runs non-blocking: ui_init() draws it, ui_update() is called every
 * 10 ms tick. printf goes to the UART; the menu draws on the OLED via &mystdout.
 * The joystick is sent to Node 2 over CAN at 50 Hz.
 *
 * AI USE: written by the candidates over several iterations. The final initialization
 * order was validated with AI (Claude), checking that every needed init is run and no
 * unnecessary ones. The main loop below is based on heavy use of AI, and then reviewed,
 * edited and validated on the hardware by the candidates.
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

int main(void)
{
    UART_Init(31);          /* 31 = 9600 baud at 4.9152 MHz */
    sram_init();
    SPI_MasterInit();
    IO_init();              /* ADC clock on PD5 + joystick button input on PB1 */
    timer_init();

    uint8_t result = can_init(CAN_MODE_NORMAL);
    if (result != 0) {
        printf("CAN initialization failed: %u\n", result);
        return 1;
    }
    mcp2515_interrupt_init();

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
            }
        }
    }
}
