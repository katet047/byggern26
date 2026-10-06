#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>

typedef struct menu menu_t;

const menu_t *menu_current(void);
uint8_t menu_item_count(const menu_t *menu);
const char *menu_item_text(const menu_t *menu, uint8_t index);
uint8_t menu_selected_index(void);

void menu_move_up(void);
void menu_move_down(void);
void menu_go_back(void);
void menu_select_current(void);

#endif
