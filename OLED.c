#include "OLED.h"
#include "fonts.h"


#define FONT_WIDTH 4
#define ASCII_OFFSET 32


#define THIS_SLAVE 1

#define DISPLAY_OFF 0xAE
#define DISPLAY_ON  0xAF
#define SET_DISPLAY_CLK 0xD5
#define SET_MUTLIPLEX_RATIO 0xA8
#define SET_DISPLAY_OFFSET 0xD3
#define DISPLAY_OFFSET  0x00
#define SET_SEGMENT_REMAP 0xA0 // evt 0xA1
#define SET_COM_OUTPUT_STAND_DIRECTION 0xC0 // evt 0xC8
#define SET_COM_PINS        0xDA
#define SET_PRECHARGE       0xD9
#define SET_VCOMH           0xDB
#define SET_MEMORY_ADDRESSING_MODE  0x20
#define ENTIRE_DISPLAY_RESUME   0xA4
#define NORMAL_DISPLAY      0xA6
#define SET_CONTRAST        0x81


#define PAGE_MODE  0x02 //Page adressing mode
#define HORIZONTAL_MODE  0x00 // A[1:0]=00b : horizontal addressing mode 

static uint8_t currLine; 
static uint8_t currCol;


void oled_init(){
    // --- hardware ---
    SPI_MasterInit();
    DDRB |= (1<<PB2); // set D/!C as output pin

    /* We are not using reset yet
    if using RES:                        // 8.8, p. 26
        RES low, wait ≥ 3 µs, RES high
    */


    // --- configuration ---
    //send DISPLAY_OFF                               // 10.12
    write_command(DISPLAY_OFF);

    //send SET_DISPLAY_CLOCK, then <divide/osc value> // 10.16 – reset value is fine to start
    write_command(SET_DISPLAY_CLK);
    write_command(0x70);                             // reset bits: 0000b and 0111b

    //send SET_MULTIPLEX, then <rows - 1>             // 10.11 – display has 64 rows
    write_command(SET_MUTLIPLEX_RATIO);
    write_command(0x3F);                                // 63 = 0x3f

    //send SET_DISPLAY_OFFSET, then <0>               // 10.15
    //send SET_START_LINE + <0>                       // 10.6 – start line is part of the command byte itself
    write_command(SET_DISPLAY_OFFSET);
    write_command(DISPLAY_OFFSET);                            //start at com 0 for now


    //send SEGMENT_REMAP <normal or flipped>          // 10.8  ┐ try both if text is mirrored
    write_command(SET_SEGMENT_REMAP);
    
    //send COM_SCAN_DIR  <normal or flipped>          // 10.14 ┘ or upside down
    write_command(SET_COM_OUTPUT_STAND_DIRECTION);

    //send SET_COM_PINS, then <config value>          // 10.18
    write_command(SET_COM_PINS);
    write_command(0x02);                                //choose alternative 1


    //send SET_CONTRAST, then <0..255>                // 10.7
    write_command(SET_CONTRAST);
    write_command(0x7F);                               // reset value

    //send SET_PRECHARGE, then <value>                // 10.17 – reset value is fine to start
    write_command(SET_PRECHARGE);
    write_command(0x22);

    //send SET_VCOMH, then <value>                    // 10.19 – reset value is fine to start
    write_command(SET_VCOMH);
    write_command(0x34);

    //send SET_MEMORY_ADDRESSING_MODE, then <page mode>  // 10.3 – page mode matches goto_line/goto_column
    write_command(SET_MEMORY_ADDRESSING_MODE);
    write_command(PAGE_MODE);

    //send ENTIRE_DISPLAY_RESUME                      // 10.9  – show RAM, not "all pixels on"
    write_command(ENTIRE_DISPLAY_RESUME);

    //send NORMAL_DISPLAY                             // 10.10 – not inverted
    write_command(NORMAL_DISPLAY);

    // --- finish ---
    oled_clear_screen();                                    // write 0x00 to all 8 pages × 128 columns
    oled_pos(0,0);//oled_home();                                     // goto line 0, column 0
    //send DISPLAY_ON                                 // 10.12 – always the last command
    write_command(DISPLAY_ON);
    stdout = &mystdout;
}


// bs[2:0]  := 000;

// cl = EXTERNAL CLOCK SCOURCE???


void write_command(uint8_t command){
    select_slave(THIS_SLAVE);
    PORTB &= ~(1<<PB2);
    SPI_transfer_n(&command, NULL, 1);
    deselect_slaves();
}

void write_data(const uint8_t *data, uint16_t len){
    select_slave(THIS_SLAVE);
    PORTB |= (1<<PB2);
    SPI_transfer_n(data, NULL, len);
    deselect_slaves();
}

void oled_goto_line(uint8_t line){
    // validate pages limits
    currLine = line%8;

        // 0xB0 let us set the page adress
        write_command(0xB0 + line%8);

}

void oled_goto_column(uint8_t column){
    currCol = column % 128; 
    // set lower 4 bits of the column adress
    write_command(0x00 + (column%128 & 0x0F ));
    // set upper 4 bits of the column adress
    // >> 4 shifts the upper 4 bits to the front so the screen can read them as a new number
    write_command(0x10 + ((column%128 >> 4) & 0x0F));

}


void oled_pos(uint8_t line,uint8_t column){
    oled_goto_line(line);
    oled_goto_column(column);

}

void incrementLine(uint8_t n){
    oled_goto_line(currLine + n);
}

void incrementCol(uint8_t n){
    oled_goto_column(currCol + n);
}

void newline(){
    oled_pos(currLine +1, 0);
}


// delete only one specific line
void oled_clear_line(uint8_t line){
    if (line <= 7) {
        uint8_t blank[128] = {0};
        oled_pos(line, 0);
        write_data(blank, sizeof(blank));
    }
}

void oled_clear_screen(void) {
    for (uint8_t line = 0; line < 8; line++){
        oled_clear_line(line);
    }
    oled_pos(0,0); // Return the cursor 
}


void oled_home(){
    oled_pos(0,0);
}

void oled_reset(){
    oled_clear_screen();
    oled_home();
}

int oled_print_char(char c, FILE *stream){
    uint8_t buffer[FONT_WIDTH];
    uint8_t index;
    if (c== '\n'){
        newline();
        return 0;
    }
    
    if (c== '\r'){
        return 0;
    }

    if (c < 32 || c > 126) {
        return 0;
    }

    index = (uint8_t)(c - ' ');
    for (uint8_t i = 0; i < FONT_WIDTH; i++) {
        buffer[i] = pgm_read_byte(&font4[index][i]);
    }
    write_data(buffer, FONT_WIDTH);
    if (currCol + FONT_WIDTH >= 128) {
        newline();
    } else {
        incrementCol(FONT_WIDTH);
    }
    return 0;
}

void oled_print_str(const char* msg){
    while (*msg)
    {
       oled_print_char(*msg++, NULL);


    }
}

FILE mystdout = FDEV_SETUP_STREAM(oled_print_char, NULL, _FDEV_SETUP_WRITE);


