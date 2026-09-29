#include "OLED.h"

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


void oled_init(){
    // --- hardware ---
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
    oled_clear();                                    // write 0x00 to all 8 pages × 128 columns
    oled_home();                                     // goto line 0, column 0
    //send DISPLAY_ON                                 // 10.12 – always the last command
    write_command(DISPLAY_ON);
}


oled_reset();
oled_home();
void oled_goto_line(line)


oled_goto_column(colum);
oled_clear_line(line);
oled_pos(row,column);
oled_print(char*){

}

oled_write()