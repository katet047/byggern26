

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdio.h>

void UART_Init(unsigned int ubrr);

//a send a character
int UART_Transmit( char data, FILE *stream);

// receive a character
int UART_Receive(FILE * stream);

//link printf
void link_printf(void);

#endif