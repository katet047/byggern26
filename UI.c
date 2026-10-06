#include "UI.h"
#include "OLED.h"
#include "IO.h"

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

typedef struct menu menu_t;
typedef void (*menu_action_t)(void);

typedef struct {
    const char *text;
    const menu_t *submenu;
    menu_action_t action;
} menu_item_t;

struct menu {
    const menu_item_t *items;
    uint8_t item_count;
    const menu_t *parent;
};

static const menu_t main_menu;
static const menu_t settings_menu;
static const menu_t help_menu;

// MENU ITEMS
static const menu_item_t main_items[] = {
    {
        .text = "Start game",
        .submenu = 0,
        .action = start_game
    },
    {
        .text = "Settings",
        .submenu = &settings_menu,
        .action = 0
    },
    {
        .text = "Help",
        .submenu = &help_menu,
        .action = 0
    }
};

static const menu_item_t settings_items[] = {
    {
        .text = "Sound",
        .submenu = 0,
        .action = change_sound
    },
    {
        .text = "Display",
        .submenu = 0,
        .action = change_display
    },
    {
        .text = "Back",
        .submenu = 0,
        .action = go_back
    }
};

static const menu_item_t help_items[] = {
    {
        .text = "Controls",
        .submenu = 0,
        .action = show_help
    },
    {
        .text = "Back",
        .submenu = 0,
        .action = go_back
    }
};

//MENUS
static const menu_t main_menu = {
    .items = main_items,
    .item_count = sizeof(main_items) / sizeof(main_items[0]),
    .parent = 0
};

static const menu_t settings_menu = {
    .items = settings_items,
    .item_count = sizeof(settings_items) / sizeof(settings_items[0]),
    .parent = &main_menu
};

static const menu_t help_menu = {
    .items = help_items,
    .item_count = sizeof(help_items) / sizeof(help_items[0]),
    .parent = &main_menu
};

static const menu_t *current_menu = &main_menu;
static uint8_t selected_index = 0;
static uint8_t scroll_lock = 0;
//static struct interface interface;


void ui_init(void) {
    oled_init();
    UI_main();
}


//ACTIONS
static void start_game(void) {
    printf("Starting game\n");
}

static void change_sound(void) {
    printf("Changing sound\n");
}

static void change_display(void) {
    printf("Changing display\n");
}

static void show_help(void) {
    printf("Showing help\n");
}

static void select_current_item(void) {
    const menu_item_t *item =
        &current_menu->items[selected_index];

    if (item->submenu != 0) {
        current_menu = item->submenu;
        selected_index = 0;
        display_menu();
    } else if (item->action != 0) {
        item->action();
    }
}

static void go_back(void) {
    if (current_menu->parent != 0) {
        current_menu = current_menu->parent;
        selected_index = 0;
        display_menu();
    }
}


// RENDERING
static void display_options(void) {
    oled_home();

    for (uint8_t i = 0; i < current_menu->item_count; i++) {
        printf("%s\n", current_menu->items[i].text);
    }
}

static void display_cursor(void) {
    for (uint8_t i = 0; i < current_menu->item_count; i++) {
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
                if (selected_index == 0) {
                    selected_index = current_menu->item_count - 1;
                } else {
                    selected_index--;
                }

                scroll_lock = 1;
                display_cursor();
            } else if (direction == DOWN) {
                selected_index =
                    (selected_index + 1) % current_menu->item_count;

                scroll_lock = 1;
                display_cursor();
            } else if (direction == LEFT) {
                go_back();
                scroll_lock = 1;
            }
        }

        if (button_pressed_event()) {
            select_current_item();
        }

        _delay_ms(10);
    }
    return 0;
}
