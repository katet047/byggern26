#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <stdio.h>



/*
• Select slave n
• Write byte
• Read byte
• Also convenient: Read/write n bytes
*/


void SPI_MasterInit();

void select_slave(uint8_t n);
void deselect_slaves();
void write_byte(uint8_t c);
uint8_t read_byte();
void SPI_transfer_n(const uint8_t *tx, uint8_t *rx, uint16_t len);









#endif