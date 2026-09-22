#ifndef adc_driver_H_
#define adc_driver_H_


#include <stdint.h>

typedef struct {
	uint16_t joy_x, joy_y, pad_x, pad_y;
} adc_readings;

typedef struct {
	uint16_t joy_x, joy_y;
} joy_pos;

typedef enum {
	NEUTRAL, LEFT, RIGHT, UP, DOWN
} joy_dir;

typedef struct {
	uint16_t pad_x, pad_y;
} pad_pos;

void ADC_driver_init();
void calibrate();
void timer0_ctc_init(void);
adc_readings read_channel();
joy_pos get_joy_pos();
pad_pos get_pad_pos();
joy_dir get_joy_dir();
#endif