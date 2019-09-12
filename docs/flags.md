# Flags

Flags are stored in F register.

## Zero

Often labeled as Z, this flag is set after an arithmetic operation result to 0.

## Substract flag

Often labeled as N, this flag is true if last operation was a substraction.

## Carry flag

Often labeled as C, this flag tells if the last arithmetic operation produced an overflow.

Code example using strong typed language and unsigned numbers:

```c
if (dest + reg < dest)
{
    F |= FFlags.C;
}
```

# Half-carry flag

Often labeled as H.

If I got it right, Gameboy ALU works on 4bit numbers. If you make arithmetic operations on 8bit numbers, it will perform 
two 4bit operations. For a 16bit operation, it perform four 4bit operations.

So on, the ALU keep track if it needs to carry a bit between each operation.

Half-carry for 8bit operation:

```c
// Keep only first four bits, make addition and check if fifth bit is set.
// If fifth bit is set, then there is half carry.
if((((A & 0xfu) + (reg & 0xfu)) & 0x10u) == 0x10u)
{
    F |= FFlags.H;
}
```

Same apply for 16bit operation:
```c
// Keep only first eleventh bits, ...
// If twelveth bit is set, then tere is half carry
if((((HL & 0xfffu) + (reg & 0xfffu)) & 0x1000u) == 0x1000u)
{
    F |= FFlags.H;
}
```
