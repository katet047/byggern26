#ifndef OLED_H_
#define OLED_H_

#include "SPI.h"


/*
• Initialisation
• Go to line
• Go to column
• Printf (or at least your own simplified version)
*/

void OLED_init();
void goToLine(uint8_t l);
void goToColum(uint8_t c);
void OledPrintf();


#endif