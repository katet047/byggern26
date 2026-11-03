#include "joystick_can.h"
#include "IO.h"
#include "can.h"

uint8_t joystick_can_send(void){
    joy_pos pos = get_joy_pos();

    can_message msg = {
        .id     = CAN_ID_JOYSTICK,
        .length = JOYSTICK_MSG_LEN,
        .data   = {
            (uint8_t)pos.joy_x,
            (uint8_t)pos.joy_y,
            (uint8_t)joy_dir_from_pos(pos),
            joy_button_pressed(),
        },
    };
    return can_send(&msg);
}
a