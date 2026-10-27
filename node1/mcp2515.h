#ifndef MCP2515_H_
#define MCP2515_H_

#include <stdint.h>

/* ---- SPI instructions (MCP2515 datasheet, chapter 12) ---- */
#define MCP_RESET        0xC0
#define MCP_READ         0x03
#define MCP_WRITE        0x02
#define MCP_RTS          0x80   /* | 0b00000TTT (TXB2 TXB1 TXB0) */
#define MCP_READ_STATUS  0xA0
#define MCP_BIT_MODIFY   0x05

#define MCP_RTS_TXB0     0x01
#define MCP_RTS_TXB1     0x02
#define MCP_RTS_TXB2     0x04

/* ---- Registers ---- */
#define MCP_CANSTAT   0x0E
#define MCP_CANCTRL   0x0F
#define MCP_CNF3      0x28
#define MCP_CNF2      0x29
#define MCP_CNF1      0x2A
#define MCP_CANINTE   0x2B
#define MCP_CANINTF   0x2C
#define MCP_EFLG      0x2D

#define MCP_TXB0CTRL  0x30
#define MCP_TXB0SIDH  0x31
#define MCP_RXB0CTRL  0x60
#define MCP_RXB0SIDH  0x61
#define MCP_RXB1CTRL  0x70
#define MCP_RXB1SIDH  0x71

/* ---- Bits ---- */
#define MCP_TXREQ     (1<<3)    /* TXBnCTRL: transmit request */
#define MCP_RX0IF     (1<<0)    /* CANINTF: RXB0 full */
#define MCP_RX1IF     (1<<1)    /* CANINTF: RXB1 full */
#define MCP_TX0IF     (1<<2)
#define MCP_RX0IE     (1<<0)    /* CANINTE: RXB0 interrupt enable */
#define MCP_RX1IE     (1<<1)    /* CANINTE: RXB1 interrupt enable */
#define MCP_RXM_ANY   0x60      /* RXBnCTRL: filters off, accept all messages */
#define MCP_BUKT      0x04      /* RXB0CTRL: rollover to RXB1 when RXB0 is full */

/* ---- Operation modes (REQOP / OPMOD, bits 7:5) ---- */
#define MCP_MODE_MASK      0xE0
#define MCP_MODE_NORMAL    0x00
#define MCP_MODE_SLEEP     0x20
#define MCP_MODE_LOOPBACK  0x40
#define MCP_MODE_LISTEN    0x60
#define MCP_MODE_CONFIG    0x80

/* CHECK: MCP2515_FOSC must be the real crystal/oscillator frequency of your MCP2515 board */
#ifndef MCP2515_FOSC
#define MCP2515_FOSC  16000000UL
#endif
/* CAN bitrate shared by Node 1 and Node 2 */
#ifndef CAN_BITRATE
#define CAN_BITRATE   125000UL
#endif

/* ---- Low-level driver ---- */
void    mcp2515_reset(void);
uint8_t mcp2515_read(uint8_t addr);
void    mcp2515_read_n(uint8_t addr, uint8_t *buf, uint8_t len);
void    mcp2515_write(uint8_t addr, uint8_t value);
void    mcp2515_write_n(uint8_t addr, const uint8_t *buf, uint8_t len);
void    mcp2515_request_to_send(uint8_t txb_mask);
uint8_t mcp2515_read_status(void);
void    mcp2515_bit_modify(uint8_t addr, uint8_t mask, uint8_t data);

/* Returns 0 if OK, 1 if it is not in config mode after reset, 2 if it did not enter 'mode' */
uint8_t mcp2515_init(uint8_t mode);

/* MCP2515 INT pin (active low) -> INT0 (PD2), falling edge */
void mcp2515_interrupt_init(void);
extern volatile uint8_t mcp2515_irq_flag;   /* set by the ISR; the ISR does NOT use SPI */

#endif
