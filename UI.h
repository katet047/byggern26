#ifndef UI_H_
#define UI_H_

void ui_init(void);
//void display_options(void);
//int UI_main(void);

static void display_options(void);
static void display_cursor(void);
static void display_menu(void);
static void select_current_item(void);
static void go_back(void);
static void start_game(void);
static void change_sound(void);
static void change_display(void);
static void show_help(void);
int UI_main(void);
#endif