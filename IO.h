#ifndef IO_H_
#define IO_H_


#include <stdint.h>

typedef struct {
	uint8_t joy_x, joy_y, pad_x, pad_y;
} adc_readings;

typedef struct {
	uint8_t joy_x, joy_y;
} joy_pos;

typedef enum {
	NEUTRAL, LEFT, RIGHT, UP, DOWN
} joy_dir;

typedef struct {
	uint8_t pad_x, pad_y;
} pad_pos;


typedef struct __attribute__((packed)) {
    union {
        uint8_t right;
        struct {
            uint8_t R1:1;
            uint8_t R2:1;
            uint8_t R3:1;
            uint8_t R4:1;
            uint8_t R5:1;
            uint8_t R6:1;
        };
    };
    union {
        uint8_t left;
        struct {
            uint8_t L1:1;
            uint8_t L2:1;
            uint8_t L3:1;
            uint8_t L4:1;
            uint8_t L5:1;
            uint8_t L6:1;
            uint8_t L7:1;
        };
    };
    union {
        uint8_t nav;
        struct {
            uint8_t NB:1;
            uint8_t NR:1;
            uint8_t ND:1;
            uint8_t NL:1;
            uint8_t NU:1;
        };
    };
} Buttons;

typedef enum {
    touchPad = 0x01,
    touchSlider = 0x02,
    joyStick = 0x03, 
    buttons = 0x04, 
    info = 0x07, 
} UserBoardRead;


void IO_init();
void calibrate();
void timer0_ctc_init(void);

void IO_read_buttons(Buttons *btn);
uint8_t button_pressed_event(Buttons *pressed);
adc_readings read_channel();
joy_pos get_joy_pos();
pad_pos get_pad_pos();
joy_dir get_joy_dir();
#endif