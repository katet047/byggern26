#include "menu.h"

#include <stdio.h>

typedef void (*menu_action_t)(void);

typedef struct menu_item {
    const char *text;
    const struct menu *submenu;
    menu_action_t action;
} menu_item_t;

struct menu {
    const menu_item_t *items;
    uint8_t item_count;
    const struct menu *parent;
};

static const struct menu main_menu;
static const struct menu settings_menu;
static const struct menu help_menu;

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

static const menu_item_t main_items[] = {
    {.text = "Start game", .submenu = 0, .action = start_game},
    {.text = "Settings", .submenu = &settings_menu, .action = 0},
    {.text = "Help", .submenu = &help_menu, .action = 0}
};

static const menu_item_t settings_items[] = {
    {.text = "Sound", .submenu = 0, .action = change_sound},
    {.text = "Display", .submenu = 0, .action = change_display},
    {.text = "Back", .submenu = 0, .action = menu_go_back}
};

static const menu_item_t help_items[] = {
    {.text = "Controls", .submenu = 0, .action = show_help},
    {.text = "Back", .submenu = 0, .action = menu_go_back}
};

static const struct menu main_menu = {
    .items = main_items,
    .item_count = sizeof(main_items) / sizeof(main_items[0]),
    .parent = 0
};

static const struct menu settings_menu = {
    .items = settings_items,
    .item_count = sizeof(settings_items) / sizeof(settings_items[0]),
    .parent = &main_menu
};

static const struct menu help_menu = {
    .items = help_items,
    .item_count = sizeof(help_items) / sizeof(help_items[0]),
    .parent = &main_menu
};

static const struct menu *current_menu = &main_menu;
static uint8_t selected_index;

const menu_t *menu_current(void) {
    return current_menu;
}

uint8_t menu_item_count(const menu_t *menu) {
    return menu->item_count;
}

const char *menu_item_text(const menu_t *menu, uint8_t index) {
    return menu->items[index].text;
}

uint8_t menu_selected_index(void) {
    return selected_index;
}

void menu_move_up(void) {
    if (selected_index == 0) {
        selected_index = current_menu->item_count - 1;
    } else {
        selected_index--;
    }
}

void menu_move_down(void) {
    selected_index = (selected_index + 1) % current_menu->item_count;
}

void menu_go_back(void) {
    if (current_menu->parent != 0) {
        current_menu = current_menu->parent;
        selected_index = 0;
    }
}

void menu_select_current(void) {
    const menu_item_t *item = &current_menu->items[selected_index];

    if (item->submenu != 0) {
        current_menu = item->submenu;
        selected_index = 0;
    } else if (item->action != 0) {
        item->action();
    }
}
