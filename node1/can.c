#include "can.h"
#include "mcp2515.h"
#include <stddef.h>

/* TODO: call the MCP2515 driver to configure the controller in 'mode' */
uint8_t can_init(uint8_t mode){
    return mcp2515_init(mode);
}

uint8_t can_send(const can_message *msg){
    uint8_t frame[13] = {0};

    if (msg == NULL || msg->id > 0x7FF || msg->length > sizeof(msg->data)) {
        return 0;
    }

    /* TX buffer 0 frame format: SIDH, SIDL, EID8, EID0, DLC, data[0..7]. */
    frame[0] = (uint8_t)(msg->id >> 3);
    frame[1] = (uint8_t)(msg->id << 5);
    frame[4] = msg->length;

    for (uint8_t i = 0; i < msg->length; i++) {
        frame[5 + i] = msg->data[i];
    }

    if (mcp2515_read(MCP_TXB0CTRL) & MCP_TXREQ) {
        return 0;
    }

    mcp2515_write_n(MCP_TXB0SIDH, frame, sizeof(frame));
    mcp2515_request_to_send(MCP_RTS_TXB0);
    return 1;
}

uint8_t can_receive(can_message *msg){
    uint8_t status;
    uint8_t frame[13];
    uint8_t rx_buffer;

    if (msg == NULL) {
        return 0;
    }

    status = mcp2515_read_status();
    if (status & MCP_RX0IF) {
        rx_buffer = MCP_RXB0SIDH;
    } else if (status & MCP_RX1IF) {
        rx_buffer = MCP_RXB1SIDH;
    } else {
        return 0;
    }

    mcp2515_read_n(rx_buffer, frame, sizeof(frame));

    msg->id = ((uint16_t)frame[0] << 3) | (frame[1] >> 5);
    msg->length = frame[4] & 0x0F;
    if (msg->length > sizeof(msg->data)) {
        msg->length = sizeof(msg->data);
    }

    for (uint8_t i = 0; i < msg->length; i++) {
        msg->data[i] = frame[5 + i];
    }

    mcp2515_bit_modify(MCP_CANINTF,
                       (status & MCP_RX0IF) ? MCP_RX0IF : MCP_RX1IF,
                       0);
    return 1;
}
