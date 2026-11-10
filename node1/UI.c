#define F_CPU 4915200UL

#include "UI.h"
#include "OLED.h"
#include "IO.h"
#include "menu.h"
#include "game.h"

#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>





#define OPTIONS 3
#define MENU_WIDTH 8*10


//struct option {
//    const char *string;
//    uint8_t row;
//};
//
//struct interface {
//    struct option options[OPTIONS];
//    uint8_t index; 
//}

static uint8_t scroll_lock = 0;

static void display_menu(void);


void ui_init(void) {
    /* oled_init() points stdout at the OLED. Keep printf on the UART instead,
     * and draw the menu explicitly with fprintf(&mystdout, ...). */
    FILE *uart_stdout = stdout;
    oled_init();
    stdout = uart_stdout;

    display_menu();
}


// RENDERING
static void display_options(void) {
    oled_home();

    const menu_t *current_menu = menu_current();
    for (uint8_t i = 0; i < menu_item_count(current_menu); i++) {
        fprintf(&mystdout, "%s\n", menu_item_text(current_menu, i));
    }
}

static void display_cursor(void) {
    const menu_t *current_menu = menu_current();
    uint8_t selected_index = menu_selected_index();

    for (uint8_t i = 0; i < menu_item_count(current_menu); i++) {
        oled_pos(i, MENU_WIDTH);

        if (i == selected_index) {
            fprintf(&mystdout, "*");
        } else {
            fprintf(&mystdout, " ");
        }
    }
}

static void display_menu(void) {
    oled_clear_screen();
    display_options();
    display_cursor();
}


/* One step of the menu: read joystick + buttons and redraw if needed.
 * Non-blocking - call it regularly (e.g. every 10 ms tick) from the main loop. */
void ui_update(void) {
    /* During a game the joystick and NB are for playing; only NR leaves the game */
    if (game_active()) {
        Buttons pressed;
        if (button_pressed_event(&pressed) && pressed.NR) {
            game_exit();
            display_menu();
        }
        return;
    }

    joy_dir direction = get_joy_dir();

    if (direction == NEUTRAL) {
        scroll_lock = 0;
    }

    if (scroll_lock == 0) {
        if (direction == UP) {
            menu_move_up();
            scroll_lock = 1;
            display_cursor();
        } else if (direction == DOWN) {
            menu_move_down();
            scroll_lock = 1;
            display_cursor();
        } else if (direction == LEFT) {
            menu_go_back();
            scroll_lock = 1;
            display_menu();
        }
    }

    Buttons pressed;
    if (button_pressed_event(&pressed)) {
        if (pressed.NB) {
            menu_select_current();
            if (!game_active()) {   /* "Start game" has drawn the game screen itself */
                display_menu();
            }
        }

        if (pressed.NR) {
            menu_go_back();
            display_menu();
        }

        /*
         * Add button-specific actions here, for example:
         *
         * if (pressed.R1) {
         *     start_game();
         * }
         */
    }
}

/* Old blocking version, kept for standalone menu testing. Not used with CAN. */
int UI_main(void) {
    display_menu();
    while (1) {
        ui_update();
        _delay_ms(10);
    }
    return 0;
}
