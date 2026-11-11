/* AI USE: the menu functionality in this file was written with help from AI (Claude),
 * and then reviewed, edited and validated by the candidates. */

#ifndef UI_H_
#define UI_H_

void ui_init(void);    /* init OLED and draw the menu (does not block) */
void ui_update(void);  /* call every 10 ms tick from the main loop */
#endif