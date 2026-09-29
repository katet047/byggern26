#include "SPI.h"
#include <avr/io.h>
#include <util/delay.h>


#define DDR_SPI DDRB
#define DD_MOSI PB5
#define DD_SCK PB7
#define DD_SS0 PB4
#define DD_SS1 PB0



void SPI_MasterInit(){
    //enable SPI
        // Port B bit 7: sett DDB7 as output (1) : enable Master clock output(SCK)
        // Miso Port b bit 6 : automatically sett as input when SPI enabled
        // MOSI port b bit 5 : set DDB5 as output : master data output
        // port b bit 4 and 0:  SS- and OC0 enable as output

    /* Set MOSI and SCK output, all others input */
    DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS0)|(1<<DD_SS1);
    /* Enable SPI, Master, set clock rate fck/16 */
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);

}



void select_slave(uint8_t n){
    //activate ss for n
    uint8_t ss0 = PORTB4;
    uint8_t ss1 = PORTB0;
    if (n== 0){
        PORTB = (0<< ss0) | (1<< ss1); // enable ss0, disable ss1
    }
    elif(n==1){
        PORTB|= (0<< ss1) | (1<<ss0);// enable ss1, disable ss0
    }

};


void write_byte(uint8_t c){
    //start clock signal sck
        // sett DDB7 as output (1)
    // write to SPI data register (SPDR)
    // wait 8 clock cycles :: one bit transmited pr cycle
        // wait for SPIF flag
    //sett ss- high ???

    // write:   
    // SPDR = n;

    SPDR = c;
    while (!(SPSR & (1 << SPIF)))
    ;



};
uint8_t read_byte(){
    // start clock cycle sck
    // read from ?? to SPDR
    // wait 8 clock cycles

    SPDR = 1;
    while (!(SPSR & (1 << SPIF)))
    ;
    return SPDR;
    
};







