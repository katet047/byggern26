#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>



/*
• Select slave n
• Write byte
• Read byte
• Also convenient: Read/write n bytes
*/


void SPI_init();

void select_slave(uint8_t n);
void write_byte(uint8_t c);
uint8_t read_byte();










#endif