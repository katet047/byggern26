#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>

/* Message layer: keep it hardware independent (reusable on Node 2) */
/* TODO: adjust the fields if you want a different message format */
typedef struct {
    uint16_t id;        /* standard 11-bit ID */
    uint8_t  length;    /* 0..8 */
    uint8_t  data[8];
} can_message;

/* Same values as REQOP in the MCP2515 (CANCTRL bits 7:5) */
#define CAN_MODE_NORMAL    0x00
#define CAN_MODE_LOOPBACK  0x40
#define CAN_MODE_LISTEN    0x60

/* TODO: initialize the CAN controller in the given mode (use loopback for now) */
uint8_t can_init(uint8_t mode);

/* TODO: send a message with the given ID, length and data */
uint8_t can_send(const can_message *msg);

/* TODO: receive a message, return 1 if one was received, 0 otherwise */
uint8_t can_receive(can_message *msg);

#endif
