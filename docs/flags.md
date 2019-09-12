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
A + reg -> regular sum
A ^ reg -> sum without carrying 
(A ^ reg) ^ (A + reg) -> keep only bits which were caried
^ 0x10 -> finally check the carry bit we're interested in
if ((((A ^ reg) ^ (A + reg)) ^ 0x10) != 0)
{
    F |= FFlags.H;
}
```

Same apply for 16bit operation:
```c
if (((HL ^ reg ^ (HL + reg)) ^ 0x1000) != 0)
{
    F |= FFlags.H;
}
```
