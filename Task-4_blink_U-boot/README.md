# Bare-Metal LED Blinky — Raspberry Pi 3B+ (AArch64)

A bare-metal application that blinks an LED connected to **GPIO 26** on the
Raspberry Pi 3B+. Built for AArch64 and loaded via U-Boot.

## Hardware Wiring

Connect an LED + current-limiting resistor (220Ω–330Ω) to GPIO 26:

```
RPi 3B+ Header
───────────────
Pin 37 (GPIO 26) ──── Resistor (220Ω) ──── LED (+) Anode
Pin 39 (GND)     ──── LED (−) Cathode
```

> **GPIO 26** = physical pin 37 on the 40-pin header.

## Build

```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../aarch64-rpi3-toolchain.cmake ..
make
```

## Deploy & Run

1. Copy `blinky.img` to the **boot partition** (FAT32) of the SD card.

2. Boot the Raspberry Pi 3B+ into U-Boot.

3. In the U-Boot serial console, run:

```
fatload mmc 0:1 0x80000 blinky.img
go 0x80000
```

1. The LED on GPIO 26 should start blinking at approximately 1 Hz.

## Files

| File                            | Description                                        |
|---------------------------------|----------------------------------------------------|
| `startup.s`                     | AArch64 assembly entry point (stack, BSS, main)    |
| `main.c`                        | GPIO configuration and blink loop                  |
| `linker.ld`                     | Linker script — loads at 0x80000                   |
| `CMakeLists.txt`                | CMake build system → `blinky.img`                  |
| `aarch64-rpi3-toolchain.cmake`  | CMake cross-compilation toolchain file             |

---

## Why is the load address `0x80000`?

On the Raspberry Pi 3B+ in 64-bit (AArch64) mode, the default kernel load address
is **`0x80000`**. This is where U-Boot (and the GPU firmware before it) expects to
place and execute the kernel image.

When we run `fatload mmc 0:1 0x80000 blinky.img`, U-Boot loads our flat
binary into RAM starting at this address. When we then run `go 0x80000`, the CPU
begins executing from that exact location. Our linker script sets `entry point to 0x80000` so
that all code and data addresses are resolved relative to this base — meaning the
binary runs correctly at the address where it is actually loaded.

If we chose a different address (say `0x40000`), we would have a mismatch: the code
would be loaded at one address but its internal references would point to another,
leading to crashes on any absolute branch or data access.

> **In short:** `0x80000` is the agreed-upon contract between U-Boot and our binary
> for where code lives in RAM on an AArch64 Raspberry Pi.

## Why do we need `startup.s`? Can't we just write everything in C?

In a **bare-metal** environment there is no OS, no C runtime, and no loader to
set things up for us. Before the first line of C code can execute, several
hardware-level tasks must be performed that **cannot be expressed in standard C**:

1. **Stack pointer setup** — C functions need a valid stack to store local variables,
   function arguments, and return addresses. On reset, the stack pointer contains an
   undefined value. We must point `sp` to a known, valid memory region before calling
   any C function. Setting a specific CPU register is an assembly-only operation.

2. **BSS zeroing** — the C standard guarantees that uninitialized global/static
   variables start at zero. In a bare-metal environment nobody does this for us.
   `startup.s` walks the `.bss` section (whose bounds are exported by the linker
   script) and writes zeros. While this *could* theoretically be done in C, it must
   happen *before* any C code runs — which is a chicken-and-egg problem.

3. **Entry point control** — the CPU starts executing at a fixed address after reset.
   The linker must place a known symbol (`_start`) at that address. Assembly gives us
   exact control over what instruction lives at byte zero of the binary.

4. **Hang loop** — if `main()` ever returns, there is nowhere to return *to*. The
   startup code provides an infinite `wfe` / branch loop that safely halts the CPU
   instead of executing garbage memory.

> **In summary:** `startup.s` is the bridge between raw hardware and the C world.
> It establishes the minimal environment (stack, zeroed BSS, entry point) that C
> requires, then hands off control to `main()`.
