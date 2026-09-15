
#include <avr/io.h>
#include <IO_driver.h>


#define ADC_BASE 0x1000


void IO_driver_init(){
    MCUCR |= (1 << SRE);
    SFIOR |= (1 << XMM2);

}


static uint8_t read_channel(uint8_t channel) {
    //pointer
    volatile uint8_t *adc_ptr = (volatile uint8_t *) (ADC_BASE);

    //start conversion
    *adc_ptr = 

    

    return *adc_ptr;
}



int get_stick_x(){

}



int get_stick_y(){

}


