#include "UI.h"
#include "OLED.h"
#include "IO.h"
#include "menu.h"

#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#define F_CPU 4915200UL




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


void ui_init(void) {
    oled_init();
    UI_main();
}


// RENDERING
static void display_options(void) {
    oled_home();

    const menu_t *current_menu = menu_current();
    for (uint8_t i = 0; i < menu_item_count(current_menu); i++) {
        printf("%s\n", menu_item_text(current_menu, i));
    }
}

static void display_cursor(void) {
    const menu_t *current_menu = menu_current();
    uint8_t selected_index = menu_selected_index();

    for (uint8_t i = 0; i < menu_item_count(current_menu); i++) {
        oled_pos(i, MENU_WIDTH);

        if (i == selected_index) {
            printf("*");
        } else {
            printf(" ");
        }
    }
}

static void display_menu(void) {
    oled_clear_screen();
    display_options();
    display_cursor();
}


int UI_main(void) {
    display_menu();

    while (1) {
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
                display_menu();
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
             *
             * if (pressed.L1) {
             *     change_sound();
             * }
             */
        }

        _delay_ms(10);
    }
    return 0;
}
