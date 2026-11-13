# Node 1: ATmega162

Joystick, buttons, OLED menu and game screen. Sends the joystick to Node 2 over CAN at 50 Hz
and keeps the game state (lives) from the misses Node 2 reports.

## Files

| File | Purpose |
|---|---|
| `main.c` | Initialization and the 100 Hz main loop (menu, game, joystick over CAN) |
| `UART.c/h` | RS-232, 9600 baud; `printf` goes here |
| `sram.c/h` | External SRAM on the address bus |
| `IO.c/h` | Joystick through the MAX156 ADC, calibration and scaling to −100..100 %, User IO board buttons over SPI (debounced) |
| `SPI.c/h` | SPI master and slave selects (OLED, IO board, MCP2515) |
| `OLED.c/h`, `fonts.h` | OLED driver; `fprintf(&mystdout, ...)` draws text |
| `UI.c/h`, `menu.c/h` | Non-blocking OLED menu (joystick up/down, NB select, NR back) |
| `mcp2515.c/h`, `can.c/h` | MCP2515 CAN controller (125 kbit/s, RX interrupt on INT0) |
| `timer.c/h` | 100 Hz tick (Timer3) |
| `joystick_can.c/h` | Joystick message to Node 2 (ID `0x10`) |
| `game.c/h` | Game modes (normal: 3 lives, endless), lives from Node 2's misses, game screen |

`sram_test.c` is the SRAM test copied from the lab manual (exercise 2). It is not part of the
build or the code submission.

## Pin layout (ATmega162, 40-pin DIP)

| Pin | Signal | Used for | | Pin | Signal | Used for |
|---:|---|---|---|---:|---|---|
| 1 | PB0 | OLED CS | | 40 | VCC | VCC |
| 2 | PB1 | Joystick button | | 39 | PA0 | AD0 |
| 3 | PB2 | OLED D/C | | 38 | PA1 | AD1 |
| 4 | PB3 | CAN (MCP2515) CS | | 37 | PA2 | AD2 |
| 5 | PB4 | IO board CS | | 36 | PA3 | AD3 |
| 6 | PB5 | MOSI | | 35 | PA4 | AD4 |
| 7 | PB6 | MISO | | 34 | PA5 | AD5 |
| 8 | PB7 | SCK | | 33 | PA6 | AD6 |
| 9 | RESET | Reset | | 32 | PA7 | AD7 |
| 10 | PD0 | RXD | | 31 | PE0 | |
| 11 | PD1 | TXD | | 30 | PE1 | ALE (latch enable) |
| 12 | PD2 | INT0 (MCP2515 interrupt) | | 29 | PE2 | |
| 13 | PD3 | | | 28 | PC7 | JTAG TDI |
| 14 | PD4 | | | 27 | PC6 | JTAG TDO |
| 15 | PD5 | ADC clock (OC1A) | | 26 | PC5 | JTAG TMS |
| 16 | PD6 | WR | | 25 | PC4 | JTAG TCK |
| 17 | PD7 | RD | | 24 | PC3 | A11 (address decoding) |
| 18 | XTAL1 | Crystal | | 23 | PC2 | A10 |
| 19 | XTAL2 | Crystal | | 22 | PC1 | A9 |
| 20 | GND | GND | | 21 | PC0 | A8 |

## Build, flash, terminal

```sh
make flash
picocom --baud 9600 --databits 8 --stopbits 2 /dev/ttyS0
```
