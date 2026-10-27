# CAN timing: 250 kbit/s

Both nodes use 250 kbit/s. Node 1 uses 16 TQ per bit; the 84 MHz Node 2
clock requires 14 TQ per bit for an exact 250 kbit/s bitrate.

## Node 1 bit timing

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

The original Arduino Due clock configuration uses an 84 MHz MCK. With the
CAN peripheral clock divided by two, that gives a 42 MHz CAN clock. A 16-TQ
configuration cannot produce exactly 250 kbit/s from 42 MHz because its
prescaler would be fractional. For the 84 MHz configuration, Node 2 uses
14 TQ instead:

```text
1 synchronization TQ
+ 2 propagation TQ
+ 6 phase segment 1 TQ
+ 5 phase segment 2 TQ
= 14 TQ
```

The resulting sample point is `(1 + 2 + 6) / 14 = 64.3%`.

The SAM3X8E timing fields are:

```text
phase2 = 5 - 1 = 4
propag = 2 - 1 = 1
phase1 = 6 - 1 = 5
sjw    = 1 - 1 = 0
brp    = 5
```

For an 84 MHz MCK configuration with the CAN peripheral clock divided by
two:

```text
MCK       = 84 MHz
CAN clock = MCK / 2 = 42 MHz
```

The SAM3X8E prescaler calculation is:

```text
BRP = CAN clock / (2 * total_TQ * bitrate) - 1
    = 42,000,000 / (2 * 14 * 250,000) - 1
    = 6 - 1
    = 5
```

The time-quanta clock is:

```text
TQ clock = 42 MHz / (2 * (5 + 1))
         = 3.5 MHz
```

Therefore:

```text
TQ duration = 285.7 ns
bit duration = 14 * 285.7 ns = 4 us
bitrate = 250 kbit/s
```

The SAM3X8E timing fields use encoded values:

```text
phase2 = 5 - 1 = 4
propag = 2 - 1 = 1
phase1 = 6 - 1 = 5
sjw    = 1 - 1 = 0
brp    = 5
```

The corresponding `CanInit` values are:

```c
CanInit can_timing = {
    .phase2 = 4,
    .propag = 1,
    .phase1 = 5,
    .sjw = 0,
    .brp = 5,
    .smp = 0
};
```

Equivalent `CAN_BR` construction is:

```c
uint32_t can_br =
      CAN_BR_PHASE2(4)
    | CAN_BR_PHASE1(5)
    | CAN_BR_PROPAG(1)
    | CAN_BR_SJW(0)
    | CAN_BR_BRP(5)
    | CAN_BR_SMP_ONCE;
```

## Comparison

| Setting | MCP2515 | ATSAM3X8E |
|---|---:|---:|
| CAN clock | 16 MHz | 42 MHz |
| Prescaler field | 1 | 5 |
| TQ clock | 4 MHz | 3.5 MHz |
| TQ duration | 250 ns | 285.7 ns |
| Propagation | 2 TQ | 2 TQ |
| Phase 1 | 7 TQ | 6 TQ |
| Phase 2 | 6 TQ | 5 TQ |
| Total | 16 TQ | 14 TQ |
| SJW | 1 TQ | 1 TQ |
| Sample point | 62.5% | 64.3% |
| Bitrate | 250 kbit/s | 250 kbit/s |
