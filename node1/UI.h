#ifndef UI_H_
#define UI_H_

void ui_init(void);    /* init OLED and draw the menu (does not block) */
void ui_update(void);  /* call every 10 ms tick from the main loop */
#endif