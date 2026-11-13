/* AI USE: written by the candidates. AI (Claude) explained how the external memory
 * interface (XMEM) works and gave an example setup, which this code is based on. */

#include "sram.h"
#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>


#define OFFSET 0x1800

void sram_init(){
	MCUCR |= (1 << SRE);
	SFIOR |= (1 << XMM2);
}

void XRAM_example(void){
	unsigned char *p = (unsigned char *) (OFFSET + 1);
	DDRC = 0xFF;
	PORTC = 0x00;
	SFIOR = (1<<XMM1) | (1<<XMM0);
	*p = 0xaa;
	SFIOR = 0x00;
	*p = 0x55;
}

void XRAM_write(unsigned int address, unsigned char value) {
    // Point to your specific target address
    unsigned char *p = (unsigned char *) address;  
    
    // Configure ports and enable External SRAM
    DDRC = 0xFF;  
    PORTC = 0x00;  
    SFIOR = (1<<XMM1) | (1<<XMM0);  // Releases interface pins if needed
    
    // Write the data to your specific address
    *p = value;  
    
    // Disable or reconfigure XRAM if required by your application
    SFIOR = 0x00;  
}
