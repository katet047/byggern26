/*
 * Ej6SqareSignal.c
 *
 * Created: 01/09/2026 13:46:24
 * Author : laura
 */ 


#define F_CPU 4915200UL

#include "UART.h"
#include "sram.h"
#include "sram_test.c"
#include "IO.h"
#include "OLED.h"
#include "UI.h"

#include <stdint.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



int main()
{

	UART_Init(31);
	sram_init();
	IO_init();
	ui_init();

	uint8_t test_pattern[8] = {
		0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81
	};
	oled_pos(0, 0);
	//write_data(test_pattern, sizeof(test_pattern));
	//oled_print_char(0,0, 'A');
	//oled_print_char(0,8, 'B');
	
	//oled_print_str("Hello world!\nSecond linekhsdfghsøfdhsldghfskjfhg");

	
	//b
	//sei();
	//a
    fdevopen(&UART_Transmit, &UART_Receive);

    //FILE * fdevopen (int(*)(char, FILE *) put, int(*)(FILE *) get)

	//UI_display();
    //printf("Hello World!\n");



	//XRAM_example();
	//SRAM_test();

	//timer0_ctc_init();






		



		


	/*		test adc reading
	while(1){delay_ms
	*/

	//printf("channel 0 is: %" PRIu8 "\n", read_channel(0));
	//printf("channel 1 is: %" PRIu8 "\n", read_channel(1));
	//printf("channel 2 is: %" PRIu8 "\n", read_channel(2));
	//printf("channel 3 is: %" PRIu8 "\n\n\n", read_channel(3));

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