#include "joystick_can.h"
#include "IO.h"
#include "can.h"
#include "game.h"

uint8_t joystick_can_send(void){
    joy_pos pos = get_joy_pos();

    /* Raw held state; Node 2 does the edge detection, so a press is never lost
     * even if ui_update() consumed the debounced press event for the menu. */
    Buttons btn;
    IO_read_buttons(&btn);

    /* Only shoot while lives are left, and only after NB has been released once, so the
     * NB press that selected "Start game" in the menu does not fire the solenoid. */
    static uint8_t shoot_armed = 0;
    uint8_t shoot = 0;
    if (!game_playing()) {
        shoot_armed = 0;
    } else if (!btn.NB) {
        shoot_armed = 1;
    } else if (shoot_armed) {
        shoot = 1;
    }

    can_message msg = {
        .id     = CAN_ID_JOYSTICK,
        .length = JOYSTICK_MSG_LEN,
        .data   = {
            (uint8_t)pos.joy_x,
            (uint8_t)pos.joy_y,
            (uint8_t)joy_dir_from_pos(pos),
            joy_button_pressed(),
            shoot,
        },
    };
    return can_send(&msg);
}
