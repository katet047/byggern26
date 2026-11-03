#ifndef JOYSTICK_CAN_H_
#define JOYSTICK_CAN_H_

#include <stdint.h>

/* CAN message with the joystick state, Node 1 -> Node 2.
 * Must match node2/joystick.h.
 *
 *   ID 0x10, DLC 4
 *   byte 0: X position, int8_t, -100..100 %
 *   byte 1: Y position, int8_t, -100..100 %
 *   byte 2: direction (joy_dir: 0 NEUTRAL, 1 LEFT, 2 RIGHT, 3 UP, 4 DOWN)
 *   byte 3: joystick button, 1 = pressed
 */
#define CAN_ID_JOYSTICK   0x10
#define JOYSTICK_MSG_LEN  4

/* Read the joystick and send it. Returns 1 if queued, 0 if TX buffer was still busy. */
uint8_t joystick_can_send(void);

#endif
