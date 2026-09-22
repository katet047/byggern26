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
#include "adc_driver.h"

#include <stdint.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



int main()
{

	UART_Init(31);
	sram_init();
	ADC_driver_init();
	//b
	//sei();
	//a
    fdevopen(&UART_Transmit, &UART_Receive);

    //FILE * fdevopen (int(*)(char, FILE *) put, int(*)(FILE *) get)

    printf("Hello World!\n");

	//XRAM_example();
	//SRAM_test();

	//timer0_ctc_init();




	while(1){
		//printf(read_channel(0));
		adc_readings data = read_channel();
		
		printf("joy x is: %" PRIu16 "\n", get_joy_pos().joy_x);
		printf("joy y is: %" PRIu16 "\n", get_joy_pos().joy_y);
		printf("pad x is: %" PRIu16 "\n", get_pad_pos().pad_x);
		printf("pad y is: %" PRIu16 "\n", get_pad_pos().pad_y);
		printf("joy dir is: %d\n", get_joy_dir());
		printf("\n");


		

		_delay_ms(2000);
	};

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