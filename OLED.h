#ifndef OLED_H_
#define OLED_H_

#include "SPI.h"


/*
• Initialisation
• Go to line
• Go to column
• Printf (or at least your own simplified version)
*/




void oled_init();
void write_command(uint8_t command);
void write_data(const uint8_t *data, uint16_t len);
void oled_reset();
void oled_home();
void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t column);
void oled_clear_line(uint8_t line);
void oled_clear_screen(void);
void oled_pos(uint8_t line, uint8_t column);
void oled_print_char(uint8_t line, uint8_t col, char c);
void oled_print_char(uint8_t line, uint8_t col, const char* msg);

#endif