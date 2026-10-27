# CAN timing: 250 kbit/s with 16 TQ

This configuration uses 250 kbit/s and 16 time quanta (TQ) per CAN bit on
both nodes.

## Common bit timing

Each CAN bit contains:

```text
1 synchronization TQ
+ 2 propagation TQ
+ 7 phase segment 1 TQ
+ 6 phase segment 2 TQ
= 16 TQ
```

The sample point is:

```text
(1 + 2 + 7) / 16 = 10 / 16 = 62.5%
```

The synchronization jump width is 1 TQ.

## Node 1: MCP2515

The MCP2515 uses a 16 MHz oscillator:

```text
FOSC = 16 MHz
bitrate = 250 kbit/s
total_TQ = 16
```

The MCP2515 bitrate formula is:

```text
bitrate = FOSC / (2 * (BRP + 1) * total_TQ)
```

Calculate the prescaler:

```text
BRP = FOSC / (2 * total_TQ * bitrate) - 1
    = 16,000,000 / (2 * 16 * 250,000) - 1
    = 2 - 1
    = 1
```

The time-quanta clock is:

```text
TQ clock = 16 MHz / (2 * (1 + 1))
         = 4 MHz
```

Therefore:

```text
TQ duration = 1 / 4 MHz = 250 ns
bit duration = 16 * 250 ns = 4 us
bitrate = 1 / 4 us = 250 kbit/s
```

The MCP2515 stores segment lengths as the actual length minus one:

```text
PRSEG  = 2 - 1 = 1
PHSEG1 = 7 - 1 = 6
PHSEG2 = 6 - 1 = 5
SJW    = 1 - 1 = 0
BRP    = 1
```

The resulting register values are:

```text
CNF1 = 0x01
CNF2 = 0xB1
CNF3 = 0x05
```

`CNF2` is calculated as:

```text
CNF2 = BTLMODE | (PHSEG1 << 3) | PRSEG
     = 0x80 | (6 << 3) | 1
     = 0xB1
```

## Node 2: ATSAM3X8E

For an 80 MHz MCK configuration with the CAN peripheral clock divided by
two:

```text
MCK       = 80 MHz
CAN clock = MCK / 2 = 40 MHz
```

The SAM3X8E prescaler calculation is:

```text
BRP = CAN clock / (2 * total_TQ * bitrate) - 1
    = 40,000,000 / (2 * 16 * 250,000) - 1
    = 5 - 1
    = 4
```

The time-quanta clock is:

```text
TQ clock = 40 MHz / (2 * (4 + 1))
         = 4 MHz
```

Therefore:

```text
TQ duration = 250 ns
bit duration = 16 * 250 ns = 4 us
bitrate = 250 kbit/s
```

The SAM3X8E timing fields use encoded values:

```text
phase2 = 6 - 1 = 5
propag = 2 - 1 = 1
phase1 = 7 - 1 = 6
sjw    = 1 - 1 = 0
brp    = 4
```

The corresponding `CanInit` values are:

```c
CanInit can_timing = {
    .phase2 = 5,
    .propag = 1,
    .phase1 = 6,
    .sjw = 0,
    .brp = 4,
    .smp = 0
};
```

Equivalent `CAN_BR` construction is:

```c
uint32_t can_br =
      CAN_BR_PHASE2(5)
    | CAN_BR_PHASE1(6)
    | CAN_BR_PROPAG(1)
    | CAN_BR_SJW(0)
    | CAN_BR_BRP(4)
    | CAN_BR_SMP_ONCE;
```

## Comparison

| Setting | MCP2515 | ATSAM3X8E |
|---|---:|---:|
| CAN clock | 16 MHz | 40 MHz |
| Prescaler field | 1 | 4 |
| TQ clock | 4 MHz | 4 MHz |
| TQ duration | 250 ns | 250 ns |
| Propagation | 2 TQ | 2 TQ |
| Phase 1 | 7 TQ | 7 TQ |
| Phase 2 | 6 TQ | 6 TQ |
| Total | 16 TQ | 16 TQ |
| SJW | 1 TQ | 1 TQ |
| Sample point | 62.5% | 62.5% |
| Bitrate | 250 kbit/s | 250 kbit/s |

The SAM3X8E must actually be configured for an 80 MHz MCK before using the
Node 2 values above. If MCK remains 84 MHz and the CAN clock remains 42 MHz,
the prescaler calculation gives 4.25, which is not a valid integer register
value for this exact 16-TQ configuration.
