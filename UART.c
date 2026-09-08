#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>




void UART_Init(unsigned int ubrr)
{
	//unsigned int ubrr = (cpu_clock /(16UL * baudrate)) -1;
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << URSEL0) | (1 << USBS0) | (1 << UCSZ01) | (3 << UCSZ00);

    link_printf();
}

int UART_Transmit( char data, FILE *stream)
{
        if (data == '\n') {
        UART_Transmit('\r', stream); // Convert newline to carriage return + newline
    }

	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<< UDRE0)))
    ;
    /* Put data into buffer, sends the data */
	UDR0 = data;

    return 0;
}

int UART_Receive(FILE * stream)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
    ;
	/* Get and return received data from buffer */
	return UDR0;
    
}

void link_printf(){
    fdevopen(&UART_Transmit, &UART_Receive);
}

//unsigned int UART_putChar(char c , FILE *stream){
//    
//}
