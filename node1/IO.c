#define F_CPU 4915200UL


#include <avr/io.h>
#include <util/delay.h>
#include "IO.h"
#include "SPI.h"
#include "OLED.h"

#define BUTTON_DEBOUNCE_SAMPLES 3

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


void IO_init(){
    MCUCR |= (1 << SRE);
    SFIOR |= (1 << XMM2);
    timer0_ctc_init();
    DDRB &= ~(1 << DDB1);      //Enable button as input
    PORTB |= (1 << PB1);    // Enable internal pull-up



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

void IO_read_buttons(Buttons *btn) {
    if (btn == 0) {
        return;
    }

    select_slave(0);
    write_byte(buttons);
    _delay_us(40);
    btn->right = read_byte();
    _delay_us(2);
    btn->left = read_byte();
    _delay_us(2);
    btn->nav = read_byte();
    deselect_slaves();
}

uint8_t button_pressed_event(Buttons *pressed) {
    static Buttons last_raw_state = {0};
    static Buttons stable_state = {0};
    static uint8_t stable_samples = 0;
    Buttons raw_state;

    if (pressed == 0) {
        return 0;
    }

    *pressed = (Buttons){0};
    IO_read_buttons(&raw_state);

    if (raw_state.right != last_raw_state.right ||
        raw_state.left != last_raw_state.left ||
        raw_state.nav != last_raw_state.nav) {
        last_raw_state = raw_state;
        stable_samples = 0;
        return 0;
    }

    if (stable_samples < BUTTON_DEBOUNCE_SAMPLES) {
        stable_samples++;
    }

    if (stable_samples >= BUTTON_DEBOUNCE_SAMPLES &&
        (raw_state.right != stable_state.right ||
         raw_state.left != stable_state.left ||
         raw_state.nav != stable_state.nav)) {
        pressed->right = raw_state.right & (uint8_t)~stable_state.right;
        pressed->left = raw_state.left & (uint8_t)~stable_state.left;
        pressed->nav = raw_state.nav & (uint8_t)~stable_state.nav;
        stable_state = raw_state;
        return pressed->right || pressed->left || pressed->nav;
    }

    return 0;
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



/* Raw ADC limits measured on our board (see jostickPosition.txt: 1 V .. 3.75 V).
 * They are widened automatically if the stick ever reads beyond them (auto-calibration). */
static uint8_t joy_x_min = 69,  joy_x_max = 248;
static uint8_t joy_y_min = 78,  joy_y_max = 240;

/* Map raw ADC value to -100..100 with separate scaling on each side of the center,
 * so 0 is exactly the calibrated rest position even if the center is not mid-range. */
static int8_t joy_axis_percent(uint8_t raw, uint8_t mid, uint8_t *min, uint8_t *max){
    if (raw < *min) *min = raw;
    if (raw > *max) *max = raw;

    int16_t value;
    if (raw >= mid){
        int16_t span = (int16_t)*max - mid;
        value = span > 0 ? ((int16_t)(raw - mid) * 100) / span : 0;
    } else {
        int16_t span = (int16_t)mid - *min;
        value = span > 0 ? -(((int16_t)(mid - raw) * 100) / span) : 0;
    }
    if (value > 100)  value = 100;
    if (value < -100) value = -100;
    return (int8_t)value;
}

joy_pos get_joy_pos(){
    adc_readings data = read_channel();

    /* If calibrate() has not been called, fall back to the middle of the measured range */
    uint8_t x_mid = joy_x_mid ? joy_x_mid : (uint8_t)((joy_x_min + joy_x_max) / 2);
    uint8_t y_mid = joy_y_mid ? joy_y_mid : (uint8_t)((joy_y_min + joy_y_max) / 2);

    return (joy_pos){
        .joy_x = joy_axis_percent(data.joy_x, x_mid, &joy_x_min, &joy_x_max),
        .joy_y = joy_axis_percent(data.joy_y, y_mid, &joy_y_min, &joy_y_max),
    };
}

uint8_t joy_button_pressed(void){
    return !(PINB & (1 << PB1));    /* JOY_B pulls to GND when pressed */
}

pad_pos get_pad_pos(){    
    adc_readings data = read_channel();
    return (pad_pos){
        .pad_x = (data.pad_x)*100/255,
        .pad_y = (data.pad_y)*100/255,
    };
}


joy_dir joy_dir_from_pos(joy_pos position){
    const int8_t threshold = 40;    /* percent from center before a direction counts */

    /* Same mapping as before: low X = LEFT, low Y = UP */
    if (position.joy_x < -threshold) return LEFT;
    if (position.joy_x >  threshold) return RIGHT;
    if (position.joy_y < -threshold) return UP;
    if (position.joy_y >  threshold) return DOWN;
    return NEUTRAL;
}

joy_dir get_joy_dir(){
    return joy_dir_from_pos(get_joy_pos());
}
