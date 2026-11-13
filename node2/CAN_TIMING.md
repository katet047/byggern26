# CAN timing: 125 kbit/s

> **AI use:** the first version of the CAN bit timing was made by the candidates but did not work. After debugging with AI (Claude) the fault was found, and the final timing on both nodes was written by AI and validated by the candidates.

Both nodes use **identical** bit timing: time quantum 500 ns, 16 TQ per bit.

| Segment | Length |
|---|---:|
| Sync | 1 TQ |
| Propagation | 2 TQ |
| Phase 1 | 7 TQ |
| Phase 2 | 6 TQ |
| **Total** | **16 TQ** |
| SJW | 1 TQ |

Sample point: (1 + 2 + 7) / 16 = 62.5%. Bit time: 16 × 500 ns = 8 µs.

## Node 1: MCP2515 (16 MHz crystal)

MCP2515 datasheet, section 5: **TQ = 2 · (BRP + 1) / FOSC**  (note the factor 2)

```text
BRP = FOSC / (2 * 16 * 125 kHz) - 1 = 16 MHz / 4 MHz - 1 = 3
TQ  = 2 * 4 / 16 MHz = 500 ns
```

Set via `CAN_BITRATE` in `node1/mcp2515.h`. Registers store length - 1:

```text
CNF1 = SJW(0)<<6 | BRP(3)                         = 0x03
CNF2 = BTLMODE | SAM(0) | PHSEG1(6)<<3 | PRSEG(1) = 0xB1
CNF3 = PHSEG2(5)                                  = 0x05
```

## Node 2: ATSAM3X8E (MCK = 84 MHz)

SystemInit: 12 MHz crystal × 14 / 2 → MCK = 84 MHz. CAN0 peripheral clock = MCK
(`PMC_PCR` DIV = 0, `PMC_PCR_DIV_PERIPH_DIV_MCK`).

SAM3X datasheet 40.7.4.1: **tCSC = (BRP + 1) / MCK**  (no factor 2)

```text
BRP + 1 = 84 MHz / (16 * 125 kHz) = 42  ->  BRP = 41
tq      = 42 / 84 MHz = 500 ns
```

```c
CAN_BR_PHASE2(5) | CAN_BR_PHASE1(6) | CAN_BR_PROPAG(1)
| CAN_BR_SJW(0)  | CAN_BR_BRP(41)   | CAN_BR_SMP_ONCE   // CAN_BR = 0x00290165
```

Datasheet limits: 8–25 TQ per bit, PHASE2 ≥ IPT (2 TQ) and ≤ PHASE1, BRP in [1, 127]. All met.

## Verifying

- Node 1 prints `CNF1=0x03 CNF2=0xB1 CNF3=0x05`.
- Node 2 prints `CAN_BR=0x00290165`; `CAN_ECR` REC/TEC should stay at 0.
- Scope: the shortest bit is 8 µs on frames from both nodes.
