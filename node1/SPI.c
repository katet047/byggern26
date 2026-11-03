#include "SPI.h"
#include <avr/io.h>

#define DDR_SPI  DDRB
#define PORT_SPI PORTB
#define DD_MOSI  PB5
#define DD_SCK   PB7

/* Slave select pins, indexed by slave number (SLAVE_IO, SLAVE_OLED, SLAVE_CAN) */
/* CHECK: the third entry (PB3) must match the pin where the MCP2515 CS is wired */
static const uint8_t ss_pin[NUM_SLAVES] = { PB4, PB0, PB3 };
#define SS_MASK ((1<<PB4) | (1<<PB0) | (1<<PB3))   /* CHECK: keep in sync with ss_pin[] */

void SPI_MasterInit(void){
    /* MOSI, SCK and all SS pins as outputs (PB4 must be an output so the AVR stays master) */
    DDR_SPI |= (1<<DD_MOSI) | (1<<DD_SCK) | SS_MASK;

    /* Deselect every slave before enabling SPI */
    deselect_slaves();

    /* SPI enable, master, mode 0, fck/16 (works for OLED and MCP2515) */
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void select_slave(uint8_t n){
    /* Raise all SS lines first, so only one slave is ever active (avoids MISO short circuit) */
    PORT_SPI |= SS_MASK;
    if (n < NUM_SLAVES){
        PORT_SPI &= ~(1 << ss_pin[n]);
    }
}

void deselect_slaves(void){
    PORT_SPI |= SS_MASK;
}

void write_byte(uint8_t c){
    SPDR = c;
    while (!(SPSR & (1 << SPIF)));
}

uint8_t read_byte(void){
    SPDR = 0x00;                        /* dummy byte generates the clock */
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

void SPI_transfer_n(const uint8_t *tx, uint8_t *rx, uint16_t len){
    for (uint16_t i = 0; i < len; i++){
        SPDR = tx ? tx[i] : 0x00;       /* tx == NULL -> send zeros */
        while (!(SPSR & (1 << SPIF)));
        if (rx) rx[i] = SPDR;           /* rx == NULL -> discard received data */
    }
}
