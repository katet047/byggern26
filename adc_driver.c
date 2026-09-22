#define F_CPU 4915200UL


#include <avr/io.h>
#include <util/delay.h>
#include "adc_driver.h"



#define ADC_BASE ((volatile uint8_t *) 0x1000)
#define WR_PORT PD6
#define RD_PORT PD7
#define MAX_CLK PD5

#define ADC_JOY_X_CH 0
#define ADC_JOY_Y_CH 1
#define ADC_PAD_X_CH 2
#define ADC_PAD_Y_CH 3

static uint8_t joy_x_mid;
static uint8_t joy_y_mid;


void ADC_driver_init(){
    MCUCR |= (1 << SRE);
    SFIOR |= (1 << XMM2);
    timer0_ctc_init();



    //DDRD |= (1 << WR_PORT) | (1 << RD_PORT);
    //PORTD |= (1 << DDD6) |(1 << DDD7);

    //sets default as inputs
    //MAX156_DATA_DDR = 0x00;
    //MAX156_DATA_PORT = 0x00;
}

void calibrate(){
    adc_readings data = read_channel();
    joy_x_mid = data.joy_x;
    joy_y_mid = data.joy_y;
}

void timer0_ctc_init(void) {
    // 1. Set PD5 (OC1A) as an output pin
    DDRD |= (1 << DDD5);

    // 2. Configure TCCR1A
    // COM1A1:0 = 01 -> Toggle OC1B on Compare Match
    // WGM12:0  = 010 -> CTC Mode (Clear Timer on Compare Match)
    TCCR1A = (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS10);

    // 3. Set the Compare Value (OCR0B)
    // Example: on a 4.9152 MHz clock with Prescaler = 1
    // OCR1A = FCPU /( 2 * FADC)-1
    // if fadc = 2457600
    // -> OCR1A = 0
    OCR1A = 0;

    // 4. Set Prescaler to 1 and start the timer
    // CS02:0 = 001 -> Clock / 1
}

adc_readings read_channel() { //uint8_t channel
    //pointer
    volatile uint8_t *adc_ptr = (volatile uint8_t *) (ADC_BASE);

    uint8_t raw_data[4];
    //start conversion
    *adc_ptr = 0x00;


    //tconv = (9*N*2)/fclk
    _delay_us(20);    //


    for (uint8_t i = 0; i<4; i++){
        raw_data[i] = *adc_ptr;
    }
    return(adc_readings){
        .joy_x= raw_data[ADC_JOY_X_CH],
        .joy_y= raw_data[ADC_JOY_Y_CH],
        .pad_x= raw_data[ADC_PAD_X_CH],
        .pad_y= raw_data[ADC_PAD_Y_CH],

    };

    //return raw_data[channel];
}



joy_pos get_joy_pos(){
    adc_readings data = read_channel();
    return(joy_pos){
        .joy_x = (data.joy_x  -69)*100 / 179,
        .joy_y = (data.joy_y - 78)*100 / 162,
    };
}

pad_pos get_pad_pos(){    
    adc_readings data = read_channel();
    return (pad_pos){
        .pad_x = (data.pad_x)*100/255,
        .pad_y = (data.pad_y)*100/255,
    };
}


joy_dir get_joy_dir(){
    joy_pos position = get_joy_pos();
    const uint16_t low_threshold = 40;
    const uint16_t high_threshold = 60;

    if (position.joy_x < low_threshold) {
        return LEFT;
    }
    if (position.joy_x > high_threshold) {
        return RIGHT;
    }
    if (position.joy_y < low_threshold) {
        return UP;
    }
    if (position.joy_y > high_threshold) {
        return DOWN;
    }

    return NEUTRAL;
}


