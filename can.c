#include "can.h"
#include "mcp2515.h"
#include <stdio.h>

/* TODO: call the MCP2515 driver to configure the controller in 'mode' */
uint8_t can_init(uint8_t mode){
    (void)mode;
    return 0;
}

/* TODO: write ID, DLC and data into a TX buffer, then request to send */
uint8_t can_send(const can_message *msg){
    (void)msg;
    return 0;
}

/* TODO: check CANINTF, read the RX buffer, clear the flag */
uint8_t can_receive(can_message *msg){
    (void)msg;
    return 0;
}
