#ifndef OLED_H_
#define OLED_H_

#include "SPI.h"


/*
• Initialisation
• Go to line
• Go to column
• Printf (or at least your own simplified version)
*/




ed_init();
oled_reset();
oled_home();
oled_goto_line(line);
oled_goto_column(colum);
oled_clear_line(line);
oled_clear_screen(void);
oled_pos(line,column);
oled_print(char*);

#endif