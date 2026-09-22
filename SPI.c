#include "SPI.h"


void SPI_init(){

}



void select_slave(uint8_t n){
    //activate ss for n
};


void write_byte(uint8_t c){
    //start clock signal sck
    // write to SPI data register (SPDR)
    // wait 8 clock cycles :: one bit transmited pr cycle

    // write:   
    // SPDR = n;


};
uint8_t read_byte(){
    // start clock cycle sck
    // read from ?? to SPDR
    // wait 8 clock cycles
    
};