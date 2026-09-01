/*
 * Ej6SqareSignal.c
 *
 * Created: 01/09/2026 13:46:24
 * Author : laura
 */ 

#define F_CPU 4915200UL   // Frequency of the connected crystal

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	// PB0 as output 
	DDRB |= (1 << PB0);

	while (1)
	{
		PORTB ^= (1 << PB0);   // Toggle the pin state
		_delay_ms(1);           // Adjust the delay to change the frequency
	}
}
