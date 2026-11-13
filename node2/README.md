# Node 2: ATSAM3X8E (Arduino Due + TTK4155 motor shield)

Receives the joystick from Node 1 over CAN and drives the game board: servo, racket motor with
PID position control, solenoid, and the IR beam that detects a lost ball.

## Files

| File | Purpose |
|---|---|
| `main.c` | Initialization and main loop: CAN in/out, IR, solenoid, joystick to servo and controller |
| `can.c/h` | CAN0 driver, 125 kbit/s (starter code, edited: standard IDs, `can_tx_ready()`) |
| `uart.c/h`, `time.c/h` | UART/`printf` and timing helpers (starter code) |
| `pwm.c/h` | Servo on PWM channel 1, 20 ms period, pulse always within 0.9–2.1 ms, deadzone/hysteresis against jitter |
| `motor.c/h` | Motor speed (PWM channel 0) and direction for the DRV8871 on the shield |
| `encoder.c/h` | Quadrature encoder on TC2 in hardware decoder mode |
| `timer_counter.c/h` | TC0 interrupt that runs the controller at 100 Hz |
| `controller.c/h` | End-stop calibration and PID position controller (anti-windup, friction compensation, deadband) |
| `solenoid.c/h` | Relay/solenoid: one short pulse per NB press, with cooldown |
| `ir.c/h` | IR beam on the ADC: moving average, self-tracking baseline, hysteresis, miss counting |
| `joystick.h`, `game.h` | CAN message formats shared with Node 1 |
| `CAN_TIMING.md` | CAN bit timing and the formulas for both nodes |

## Pins (from the TTK4155 Motor Shield documentation, v26)

| Function | Due pin | SAM3X pin | Peripheral |
|---|---|---|---|
| Motor PWM | D20 | PB12 | PWM channel 0 (PWMH0) |
| Motor direction | D7 | PC23 | GPIO |
| Servo | D21 | PB13 | PWM channel 1 (PWMH1) |
| Encoder A / B | D5 / D4 | PC25 / PC26 | TC2 channel 0 (quadrature decoder) |
| Solenoid relay (active low) | D9 | PC21 | GPIO → 1 kΩ → MPSA56 → relay |
| IR photodiode (via op-amp) | A0 | PA16 | ADC channel 7 |
| CAN | CANRX0 / CANTX0 | PA1 / PA0 | CAN0 |

## Settings worth knowing

- `USE_POSITION_CONTROL` in `main.c`: 1 = PID position control, 0 = open loop (exercise 8.3).
- `PRINT_IR_DEBUG` in `main.c`: 1 prints the IR reading for tuning; keep 0 when playing.
- IR thresholds in `ir.h`, controller gains and friction compensation in `controller.c`.

## Build, flash, terminal

Needs the course handout folder `sam/` (not part of the code submission).

```sh
make flash
picocom --baud 9600 --databits 8 --stopbits 2 /dev/ttyACM1
```
