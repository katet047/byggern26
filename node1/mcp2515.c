#ifndef F_CPU
#define F_CPU 4915200UL
#endif

#include "mcp2515.h"
#include "SPI.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t mcp2515_irq_flag = 0;

/* Bit timing: 16 TQ per bit -> Sync=1, Prop=2, PS1=7, PS2=6 (sample point ~62.5%) */
/* CHECK: if the crystal or bitrate changes, make sure BRP is still a valid integer (0..63) */
#define CAN_BRP   ((MCP2515_FOSC / (2UL * 16UL * CAN_BITRATE)) - 1)
#define CNF1_VAL  ((uint8_t)CAN_BRP)                    /* SJW = 1 TQ, BRP */
#define CNF2_VAL  (0x80 | ((7-1) << 3) | (2-1))         /* BTLMODE=1, PHSEG1=7, PRSEG=2 */
#define CNF3_VAL  (6-1)                                 /* PHSEG2=6 */

void mcp2515_reset(void){
    select_slave(SLAVE_CAN);
    write_byte(MCP_RESET);
    deselect_slaves();
    _delay_ms(2);               /* wait for the oscillator start-up */
}

uint8_t mcp2515_read(uint8_t addr){
    uint8_t v;
    select_slave(SLAVE_CAN);
    write_byte(MCP_READ);
    write_byte(addr);
    v = read_byte();
    deselect_slaves();
    return v;
}

void mcp2515_read_n(uint8_t addr, uint8_t *buf, uint8_t len){
    select_slave(SLAVE_CAN);
    write_byte(MCP_READ);
    write_byte(addr);
    for (uint8_t i = 0; i < len; i++){
        buf[i] = read_byte();   /* address auto-increments while CS stays low */
    }
    deselect_slaves();
}

void mcp2515_write(uint8_t addr, uint8_t value){
    select_slave(SLAVE_CAN);
    write_byte(MCP_WRITE);
    write_byte(addr);
    write_byte(value);
    deselect_slaves();
}

void mcp2515_write_n(uint8_t addr, const uint8_t *buf, uint8_t len){
    select_slave(SLAVE_CAN);
    write_byte(MCP_WRITE);
    write_byte(addr);
    for (uint8_t i = 0; i < len; i++){
        write_byte(buf[i]);
    }
    deselect_slaves();
}

void mcp2515_request_to_send(uint8_t txb_mask){
    select_slave(SLAVE_CAN);
    write_byte(MCP_RTS | (txb_mask & 0x07));
    deselect_slaves();
}

uint8_t mcp2515_read_status(void){
    uint8_t s;
    select_slave(SLAVE_CAN);
    write_byte(MCP_READ_STATUS);
    s = read_byte();
    deselect_slaves();
    return s;
}

void mcp2515_bit_modify(uint8_t addr, uint8_t mask, uint8_t data){
    /* Only works on registers that support bit modify (CANCTRL, CANINTF, CNFx, ...) */
    select_slave(SLAVE_CAN);
    write_byte(MCP_BIT_MODIFY);
    write_byte(addr);
    write_byte(mask);
    write_byte(data);
    deselect_slaves();
}

uint8_t mcp2515_init(uint8_t mode){
    mcp2515_reset();

    /* After reset the MCP2515 is in configuration mode (CANSTAT = 0x80) */
    if ((mcp2515_read(MCP_CANSTAT) & MCP_MODE_MASK) != MCP_MODE_CONFIG){
        return 1;   /* CHECK: if this fails, check power, clock, reset, CS and SPI wiring */
    }

    /* Bit timing (can only be written in config mode) */
    mcp2515_write(MCP_CNF1, CNF1_VAL);
    mcp2515_write(MCP_CNF2, CNF2_VAL);
    mcp2515_write(MCP_CNF3, CNF3_VAL);

    /* Receive buffers: accept everything, RXB0 rolls over into RXB1 */
    mcp2515_write(MCP_RXB0CTRL, MCP_RXM_ANY | MCP_BUKT);
    mcp2515_write(MCP_RXB1CTRL, MCP_RXM_ANY);

    /* Clear pending flags and enable RX interrupts */
    mcp2515_write(MCP_CANINTF, 0x00);
    mcp2515_write(MCP_CANINTE, MCP_RX0IE | MCP_RX1IE);

    /* Leave config mode: request the wanted mode (e.g. loopback) */
    mcp2515_bit_modify(MCP_CANCTRL, MCP_MODE_MASK, mode);

    /* The mode change is not instant, so poll CANSTAT */
    for (uint8_t i = 0; i < 100; i++){
        if ((mcp2515_read(MCP_CANSTAT) & MCP_MODE_MASK) == mode) return 0;
        _delay_us(50);
    }
    return 2;
}

void mcp2515_interrupt_init(void){
    /* CHECK: INT0 = PD2 is my assumption; if your MCP2515 INT is on INT1 (PD3), use INT1 / ISC11 / INTF1 / INT1_vect instead */
    DDRD  &= ~(1 << PD2);       /* INT0 as input */
    PORTD |=  (1 << PD2);       /* pull-up */
    MCUCR |=  (1 << ISC01);     /* falling edge (ISC01=1, ISC00=0) */
    MCUCR &= ~(1 << ISC00);
    GIFR   =  (1 << INTF0);     /* clear any pending flag */
    GICR  |=  (1 << INT0);      /* enable INT0 */
}

ISR(INT0_vect){
    /* Keep the ISR short: no SPI here, otherwise it could cut a transfer in the middle */
    mcp2515_irq_flag = 1;
}
