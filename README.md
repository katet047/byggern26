# byggern26

How to run: 
make flash # builds and sends the code to the avr


sudo picocom --baud 9600 --databits 8 --stopbits 2 /dev/ttyS0 # see output from avr in picocom terminal


for lab 2.2: 
    - de-cross the adress and data cables to the SRAM
    - put lowpass filter on the ALE signal
    - fix star ground and voltage supply




Writning to pins:



| Action | C Syntax Code | Explanation |
|---|---|---|
| Define Output | `DDRB \|= (1 << DDB5);` | Configures PB5 to push electricity out. |
| Define Input | `DDRB &= ~(1 << DDB7);` | Configures PB7 to read sensor/button data. |
| Set Pin High | `PORTB \|= (1 << PB5);` | Drives the pin to VCC voltage. |
| Set Pin Low | `PORTB &= ~(1 << PB5);` | Drives the pin to Ground (0V). |
| Read Input Pin | `if (PINB & (1 << PINB7))` | Checks if a voltage is currently applied to PB7. |







TODO: 
    - interface mouse not cleared after joystick movement
    - implemnet button 
