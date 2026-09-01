/*
 * Ej6SqareSignal.c
 *
 * Created: 01/09/2026 13:46:24
 * Author : laura
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#define F_CPU 4915200UL   // Frequency of the connected crystal


int main(void)
{
	UART_Init(103);
	
	//b
	sei();
	
	//a
	UART_Transmit('O');
	UART_Transmit('K');
	UART_Transmit('\n');
	
	while(1) {
		
	}
}




// ex3.5
// #define F_CPU 4915200UL   // Frequency of the connected crystal
{
	// PB0 as output
	DDRB |= (1 << PB0);

	while (1)
	{
		PORTB ^= (1 << PB0);   // Toggle the pin state
		_delay_ms(1);           // Adjust the delay to change the frequency
	}
}	

{
	UART_Init(103);
	
	//b
	sei();
	
	//a
	UART_Transmit('O');
	UART_Transmit('K');
	UART_Transmit('\n');
	
	while(1) {
		
	}
}