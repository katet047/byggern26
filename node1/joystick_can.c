#include "joystick_can.h"
#include "IO.h"
#include "can.h"

uint8_t joystick_can_send(void){
    joy_pos pos = get_joy_pos();

    /* Raw held state; Node 2 does the edge detection, so a press is never lost
     * even if ui_update() consumed the debounced press event for the menu. */
    Buttons btn;
    IO_read_buttons(&btn);

    can_message msg = {
        .id     = CAN_ID_JOYSTICK,
        .length = JOYSTICK_MSG_LEN,
        .data   = {
            (uint8_t)pos.joy_x,
            (uint8_t)pos.joy_y,
            (uint8_t)joy_dir_from_pos(pos),
            joy_button_pressed(),
            btn.NB,
        },
    };
    return can_send(&msg);
}
