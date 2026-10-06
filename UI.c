#include "UI.h"
#include "OLED.h"
#include "IO.h"
#include <stdint.h>
#include <stdio.h>

/* PLan: 
make interface[]: 
    struct: {string, row} option 

    navigation; 
        acces joystik input
        move 'cursor' on interface
        
later: buttons 
    acces button input and poll for events



*/

#define OPTIONS 3


struct option {
    const char *string;
    uint8_t row;
};

struct interface {
    struct option options[OPTIONS];
    uint8_t index; 
};

static struct interface interface;
static uint8_t scroll_lock = 0;

void ui_init(void) {
    interface.index = 0;
    static char option_text[OPTIONS][16];
    
    for(int i = 0; i < OPTIONS; i++){
        interface.options[i].row= i; 
        // static char option_text[OPTIONS][16];

        snprintf(option_text[i], sizeof(option_text[i]), "option %d", i);
        interface.options[i].string = option_text[i];
    }

    oled_init();
    UI_main();
}

void UI_display(){
    oled_clear_screen(); 
    oled_home();         
    
    for(int i = 0; i < OPTIONS; i++){
        printf("%s", interface.options[i].string);
        
        if(i == interface.index){
            printf("  *");
        }
        printf("\n"); 
    }
}


int UI_main(){
    UI_display(); 
    
    while (1)
    {
        joy_dir joy_dir = get_joy_dir(); 
        
        if (joy_dir == NEUTRAL) {
            scroll_lock = 0;
        }
        
        if (scroll_lock == 0) {
            
            if(joy_dir == UP){
                interface.index = (interface.index == 0) ? OPTIONS - 1 : interface.index - 1;
                scroll_lock = 1; 
                UI_display();   
            }
            else if(joy_dir == DOWN){
                interface.index = (interface.index + 1) % OPTIONS;
                scroll_lock = 1;
                UI_display();   
            }
        }
        
        _delay_ms(10);
    }

    // we need to return the menu position when the joystick button is clicked???

    return 0;
}
