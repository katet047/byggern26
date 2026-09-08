/*
 * Ej6SqareSignal.c
 *
 * Created: 01/09/2026 13:46:24
 * Author : laura
 */ 


#define F_CPU 4915200UL

#include "UART.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


int main()
{

	UART_Init(31);
	
	//b
	//sei();
	//a
    fdevopen(&UART_Transmit, &UART_Receive);

    //FILE * fdevopen (int(*)(char, FILE *) put, int(*)(FILE *) get)

    printf("Hello World!\n");


	//while(1) {
    //UART_Transmit('O');
	//UART_Transmit('K');
	//UART_Transmit('\n');
    //_delay_ms(200);
    //}
}




// ex3.5
// #define F_CPU 4915200UL   // Frequency of the connected crystal
//{
//	// PB0 as output
//	DDRB |= (1 << PB0);
//
//	while (1)
//	{
//		PORTB ^= (1 << PB0);   // Toggle the pin state
//		_delay_ms(1);           // Adjust the delay to change the frequency
//	}
//}	