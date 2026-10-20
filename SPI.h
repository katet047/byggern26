#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>

/* Slave select numbering (the n in select_slave) */
#define SLAVE_IO    0   /* User board (buttons, joystick) -> PB4 */
#define SLAVE_OLED  1   /* OLED                           -> PB0 */
#define SLAVE_CAN   2   /* MCP2515                        -> PB3 */  /* CHECK: PB3 is my assumption, change it if your CS wiring is different */
#define NUM_SLAVES  3

void SPI_MasterInit(void);

void select_slave(uint8_t n);
void deselect_slaves(void);
void write_byte(uint8_t c);
uint8_t read_byte(void);
void SPI_transfer_n(const uint8_t *tx, uint8_t *rx, uint16_t len);

#endif
