#pragma once

#include <stdint.h>
#include "can.h"

/* Joystick message from Node 1. Must match node1/joystick_can.h.
 *   ID 0x10, DLC 4
 *   byte 0: X position, int8_t, -100..100 %
 *   byte 1: Y position, int8_t, -100..100 %
 *   byte 2: direction (0 NEUTRAL, 1 LEFT, 2 RIGHT, 3 UP, 4 DOWN)
 *   byte 3: joystick button, 1 = pressed
 */
#define CAN_ID_JOYSTICK   0x10
#define JOYSTICK_MSG_LEN  4

typedef enum {
    JOY_NEUTRAL, JOY_LEFT, JOY_RIGHT, JOY_UP, JOY_DOWN
} JoyDir;

typedef struct {
    int8_t  x;      // -100..100 %
    int8_t  y;      // -100..100 %
    JoyDir  dir;
    uint8_t button; // 1 = pressed
} Joystick;

// Decode a CAN message into a Joystick. Returns 1 if it was a valid joystick message.
static inline uint8_t joystick_from_can(const CanMsg *m, Joystick *joy){
    if (m->id != CAN_ID_JOYSTICK || m->length < JOYSTICK_MSG_LEN) {
        return 0;
    }
    joy->x      = (int8_t)m->byte[0];
    joy->y      = (int8_t)m->byte[1];
    joy->dir    = (m->byte[2] <= JOY_DOWN) ? (JoyDir)m->byte[2] : JOY_NEUTRAL;
    joy->button = m->byte[3] ? 1 : 0;
    return 1;
}
