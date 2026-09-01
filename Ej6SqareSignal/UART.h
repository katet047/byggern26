

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void UART_Init(unsigned int ubrr);

//a send a character
void UART_Transmit(char data);

// receive a character
char UART_recibe(void);

#endif