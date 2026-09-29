#include "OLED.h"



// bs[2:0]  := 000;

// cl = EXTERNAL CLOCK SCOURCE???


write_command();
write_data();



void oled_init(){


    DDRB |= (1<<PB2) // set D/!C as output pin

    // DATASHEET COMMANDS
    write_command(0xAE);  // dislay OFF

    write_command(0xD5); // 
    write_command(0x7F);

    write_command();
    write_command();
    write_command();
    write_command();
    write_command();
    write_command();
    write_command();
    write_command();
    write_command();
}



// oled_reset();
// oled_home();




void oled_goto_line(uint8_t line){
    // validate pages limits
    if (line <= 7) {
        // 0xB0 let us set the page adress
        write_command(0xB0 + line);
    }

}




void oled_goto_column(uint8_t column){
    if (column <= 127) {
        // set lower 4 bits of the column adress
        write_command(0x00 + (column & 0x0F ));
        // set upper 4 bits of the column adress
        // >> 4 shifts the upper 4 bits to the front so the screen can read them as a new number
        write_command(0x10 + ((column >> 4) & 0x0F));

    }
}


void oled_pos(uint8_t line,uint8_t column){
    oled_goto_line(line);
    oled_goto_column(column);

}



// delete only one specific line
void oled_clear_line(uint8_t line){
        if (line <= 7) {
        oled_pos(line, 0);

        for (uint8_t col = 0; col < 128; col++) {
            write_data(0x00)
        }    
    }
}

void oled_clear_screen(void) {
    for (uint8_t line = 0; line < 8; line++){
        oled_clear_line(line);
    }
    oled_pos(0,0); // Return the cursor 
}

void oled_print(char*){

}

