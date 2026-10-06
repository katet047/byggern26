# byggern26

How to run: 
make flash # builds and sends the code to the avr


sudo picocom --baud 9600 --databits 8 --stopbits 2 /dev/ttyS0 # see output from avr in picocom terminal

    TODO:
    - fix star ground and voltage supply
    - change how joystick data is read and written, change to using SPI commands


## TODO: Implement SPI-based button input

The user IO board exposes button states through SPI, but the project originally read only the joystick-center button directly from `PB1`. This prevented the other buttons from being accessed consistently and made it difficult to assign separate actions to each button.

Use the IO board's SPI commands:

- `0x03` (`joyStick`) returns joystick `X`, `Y`, and center-button state.
- `0x04` (`buttons`) returns the right-side, left-side, and navigation button states.

Each SPI read must:

1. Select the IO-board AVR.
2. Send the command byte.
3. Wait at least `40 µs`.
4. Read the response bytes.
5. Wait at least `2 µs` between response bytes.
6. Deselect the AVR.

Implement the following:

- `IO_read_buttons(Buttons *btn)` for the `0x04` command.
- A joystick-reading function using `0x03` that returns `X`, `Y`, and the joystick-center button.
- Debounced press-event functions that report only the transition from released to pressed.
- Button-specific actions in the UI, for example mapping `NB` to menu selection, `NL` to going back, and assigning `R1`, `L1`, `NU`, etc. to other actions.

Relevant files:

- [`IO.c`](/home/student/byggern26/IO.c)
- [`IO.h`](/home/student/byggern26/IO.h)
- [`SPI.c`](/home/student/byggern26/SPI.c)
- [`SPI.h`](/home/student/byggern26/SPI.h)
- [`UI.c`](/home/student/byggern26/UI.c)
- [User IO-board documentation](/home/student/byggern26/Datasheets/TTK4155%20User%20IO-board.html)




Writning to pins:



| Action | C Syntax Code | Explanation |
|---|---|---|
| Define Output | `DDRB \|= (1 << DDB5);` | Configures PB5 to push electricity out. |
| Define Input | `DDRB &= ~(1 << DDB7);` | Configures PB7 to read sensor/button data. |
| Set Pin High | `PORTB \|= (1 << PB5);` | Drives the pin to VCC voltage. |
| Set Pin Low | `PORTB &= ~(1 << PB5);` | Drives the pin to Ground (0V). |
| Read Input Pin | `if (PINB & (1 << PINB7))` | Checks if a voltage is currently applied to PB7. |








## AVR pin layout

ATmega162 40-pin DIP layout. Add the purpose of each pin in the **Used for** columns.

| Left pin | Left signal | Used for | ATmega162 | Right pin | Right signal | Used for |
|---:|---|---|:---:|---:|---|---|
| 1 | PB0 |Disp CS  |  | 40 | VCC |VCC  |
| 2 | PB1 |JButton  |  | 39 | PA0 |ADR  |
| 3 | PB2 |D/c⁻ sig |  | 38 | PA1 |ADR  |
| 4 | PB3 |CAN CS  |  | 37 | PA2 |ADR  |
| 5 | PB4 |IO CS  |  | 36 | PA3 |ADR  |
| 6 | PB5 |MOSI  |  | 35 | PA4 |ADR  |
| 7 | PB6 |MISO  |  | 34 | PA5 |ADR  |
| 8 | PB7 |SCK  |  | 33 | PA6 |ADR  |
| 9 | RESET |RESET  |  | 32 | PA7 |ADR  |
| 10 | PD0 |RXD  |  | 31 | PE0 |  |
| 11 | PD1 |TXD  |  | 30 | PE1 |Lach Enable  |
| 12 | PD2 |  |  | 29 | PE2 |  |
| 13 | PD3 |  |  | 28 | PC7 |JTAG 9  |
| 14 | PD4 |  |  | 27 | PC6 |JTAG 3  |
| 15 | PD5 |ADC clock  |  | 26 | PC5 |JTAG 5  |
| 16 | PD6 |WR  |  | 25 | PC4 |JTAG 4  |
| 17 | PD7 |RD  |  | 24 | PC3 |ADR swaps SRAM   |
| 18 | XTAL1 |MCU CLK  |  | 23 | PC2 |ADR  |
| 19 | XTAL2 |MCU clk  |  | 22 | PC1 |ADR  |
| 20 | GND |GND  |  | 21 | PC0 |ADR  |
