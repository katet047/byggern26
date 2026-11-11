https://github.com/katet047/byggern26

# TTK4155 ping-pong game, H26

Distributed, computer-controlled ping-pong game for TTK4155 Embedded and Industrial Computer
Systems Design at NTNU. Two nodes communicate over CAN at 125 kbit/s.

| Folder | Node | MCU | Responsibility |
|---|---|---|---|
| [`node1/`](node1/README.md) | Node 1 | ATmega162 (breadboard + User IO board) | Joystick through the external ADC, buttons, OLED menu and game screen, game logic (lives) |
| [`node2/`](node2/README.md) | Node 2 | ATSAM3X8E (Arduino Due + motor shield) | Servo, motor with encoder and PID position control, solenoid, IR beam (ball lost) |

## How the game works

1. Choose **Start game** (3 lives, game over at 0) or **Endless** (lives keep counting below 0)
   in the OLED menu with the joystick and the NB button.
2. Joystick **x** sets the racket position (closed-loop PID on Node 2), joystick **y** sets
   the servo, and **NB** fires the solenoid.
3. A missed ball rolls into the bottom corner and breaks the IR beam. Node 2 counts the miss
   and sends it to Node 1, which updates the lives on the OLED.
4. **NR** goes back to the menu.

## CAN messages

| ID | Direction | DLC | Content |
|---|---|---|---|
| `0x10` | Node 1 → Node 2 | 5 | Joystick x, y (−100..100 %), direction, joystick button, NB (shoot) |
| `0x20` | Node 2 → Node 1 | 2 | Misses since game start (`uint16_t`, low byte first) |
| `0x30` | Node 1 → Node 2 | 0 | Start a new game (misses back to 0) |

Bit timing and formulas: [`node2/CAN_TIMING.md`](node2/CAN_TIMING.md).

## Building and flashing

Node 1 (avr-gcc, Atmel-ICE over JTAG) and Node 2 (arm-none-eabi-gcc, OpenOCD):

```sh
cd node1 && make flash
cd node2 && make flash
```

Node 2's Makefile needs the course handout folder `node2/sam/` (CMSIS headers, startup code,
linker script, OpenOCD config). It is left out of the code submission because it is unmodified
handout code; it is in the Git repository.

Serial terminals (9600 baud):

```sh
picocom --baud 9600 --databits 8 --stopbits 2 /dev/ttyS0      # Node 1 (RS-232)
picocom --baud 9600 --databits 8 --stopbits 2 /dev/ttyACM1    # Node 2 (Arduino Due USB)
```

## Code we did not write ourselves

- **AI:** the last part of exercise 7 (IR) and most of exercise 8 (motor, encoder, PID
  controller, solenoid), the servo jitter fix and the game system were written with heavy use
  of AI (Claude), and then reviewed, edited and validated on the hardware by us.
- Every file or section with AI use or copied code is marked with an `AI USE` or `SOURCE`
  comment at the top of the file or next to the code.
- **AI, CAN:** the CAN/MCP2515 driver on Node 1 was mostly written with AI. We made the first
  version of the bit timing, but it did not work; after debugging with AI the fault was found
  and the final timing on both nodes was written by AI and validated by us.
- **AI, other Node 1 help:** `SPI.c/h` was written by us with help and direction from AI, and
  the comments were polished by AI. `timer.c/h` was drafted by AI and we filled in the register
  values. For `sram.c/h`, AI explained how the external memory interface works and gave an
  example setup; the code was written by us.
- **AI, Node 1 main:** `node1/main.c` went through several iterations by us; the final
  initialization was validated with AI, and the main loop is based on heavy use of AI.
- **AI, menu:** Node 1's menu (`menu.c/h`, `UI.c/h`) was written with help from AI (Claude),
  and then reviewed, edited and validated by us.
- **AI, final cleanup:** removing unused code, rewriting comments and writing the README files
  was done with help from AI (Claude), and reviewed by us.
- **Course handouts and datasheets:** `node1/fonts.h` is copied unchanged from the TTK4155
  course files. `node1/UART.c` is based on the USART example code in the ATmega162 datasheet.
  The SRAM test from the lab manual (`node1/sram_test.c`) is in the repository but not in the
  code submission. Node 2's `can.c/h`, `time.c/h`, `uart.c/h` and `Makefile` started from the
  TTK4155 node 2 starter code and were edited by us. `node2/sam/` is unmodified handout code.
