# U-Boot Build and Deployment - [Task - 3]

---

# Part A: U-Boot Build and Deployment

## 1. What is a Bootloader?

A **bootloader** is a small program that executes immediately after the system is powered on. Its primary responsibilities include:

- Initializing essential hardware components such as CPU, RAM, and peripherals.
- Preparing the execution environment.
- Loading the operating system kernel into memory.
- Transferring execution control to the kernel.

Without a bootloader, the system cannot start the operating system.

---

## 2. Raspberry Pi 3B+ Boot Chain

The Raspberry Pi uses a GPU-first boot process, which differs from standard PC systems.

### Boot Sequence

```
Power On
   ↓
BootROM (GPU)
   ↓
bootcode.bin
   ↓
start.elf
   ↓
config.txt
   ↓
u-boot.bin
   ↓
Linux Kernel
```

### Explanation of Each Stage

**1. Power On**  
The GPU starts execution before the ARM CPU.

**2. BootROM**  

- Hardcoded inside the GPU.
- Reads the SD card.
- Loads the second-stage bootloader.

**3. bootcode.bin**  

- Initializes SDRAM.
- Loads the GPU firmware.

**4. start.elf**  

- Main GPU firmware.
- Reads system configuration from `config.txt`.
- Loads the next stage.

**5. U-Boot**  

- U-Boot is loaded and the loads the Linux Kernel.

**6. Linux Kernel**  

- Initializes the operating system.
- Mounts the root filesystem.
- Starts user-space processes.

---

## 3. PC Boot Chain

Standard PCs follow a firmware-first boot process.

### Boot Sequence

```
Power On
   ↓
BIOS / UEFI
   ↓
MBR / GPT
   ↓
GRUB Bootloader ---------
   ↓                    ↓
Linux Kernel     Windows Bootloader
                        ↓
                 Windows Kernel
```

### Explanation

**1. BIOS / UEFI**  

- Performs POST (Power-On Self-Test).
- Initializes hardware components.

**2. MBR / GPT**  

- Contains partition table and bootloader location.

**3. GRUB Bootloader**  

- Loads Linux kernel and initramfs.
- Provides boot selection menu.

**4. Linux Kernel**  

- Initializes system.
- Starts init system.

---

## 4. Difference Between U-Boot and GRUB

| Feature | U-Boot | GRUB |
|--------|--------|------|
| Target Systems | Embedded Systems | Desktop and Servers |
| Hardware Initialization | Yes | No |
| Depends on BIOS/UEFI | No | Yes |
| Primary Role | Hardware init + load kernel | OS selection and kernel loading |
| Customization | Highly customizable | Limited |
| Size | Lightweight | Larger |

---

## 5. Required Files in Raspberry Pi Boot Partition for U-Boot

| File | Purpose |
|-----|---------|
| bootcode.bin | Initializes SDRAM and loads GPU firmware |
| start.elf | GPU firmware responsible for system setup |
| config.txt | Boot configuration file (specifies kernel=u-boot.bin) |
| u-boot.bin | The U-Boot bootloader binary |
| bcm2710-rpi-3-b-plus.dtb | Device Tree Blob describing hardware |

---

## 6. Hardware Description File and Loading Stage

The hardware description is provided by the **Device Tree Blob (.dtb)** file.

Example:

```
bcm2710-rpi-3-b-plus.dtb
```

### When it is loaded

- Usually loaded by the GPU firmware (`start.elf`) before U-Boot starts.
- Alternatively, U-Boot can load it manually before booting the kernel.

The DTB allows the kernel to understand hardware layout without hardcoding it.

---

## 7. How Linux Identifies Partitions Using losetup

When using:

```
losetup --partscan sd.img
```

The Linux kernel performs the following:

1. Reads the partition table from the image file.
2. The partition table is located in the first sector (MBR or GPT).
3. Extracts partition offsets and sizes.
4. Creates virtual block devices such as:

```
/dev/loop0p1
/dev/loop0p2
```

This allows mounting individual partitions from the image file.

---

# Part B: U-Boot Commands and Environment

## 1. bdinfo Command

The `bdinfo` command displays board-specific information such as:

- DRAM start address
- DRAM size
- Relocation address
- CPU frequency
- Memory layout

Example:

```
bdinfo
```

---

## 2. printenv Command

The `printenv` command displays all U-Boot environment variables.

Example:

```
printenv
```

Common variables include:

- bootcmd
- bootargs
- serverip
- ipaddr

---

## 3. DRAM Start Address

For Raspberry Pi 3B+ (AArch64):

```
0x00000000
```

This is the physical base address of RAM.

---

## 4. Difference Between run and go Commands

### run

- Executes commands stored in environment variables.

Example:

```
run bootcmd
```

### go

- Directly jumps to a memory address and executes code.

Example:

```
go 0x8000
```

---

## 5. bootargs Purpose

`bootargs` is an environment variable containing Linux kernel command-line parameters.

Example:

```
bootargs=console=ttyS0 root=/dev/mmcblk0p2 rw
```

### Purpose

- Passed from U-Boot to Linux kernel.
- Used by the kernel to configure:
  - Console
  - Root filesystem
  - Debug options
  - Boot behavior

---

## 6. Why Kernel is Loaded at 0x62000000 Instead of 0x60000000

Kernel load addresses must avoid memory conflicts.

### Reasons

**1. Avoid Memory Overlap**  
The kernel requires space for:

- Decompression
- Device Tree Blob
- Boot parameters

**2. Avoid U-Boot Memory Region**  
U-Boot relocates itself into RAM. Loading the kernel too close may overwrite critical data.

**3. Safe Memory Offset**  
Using:

```
0x62000000
```

provides safe separation between:

- U-Boot
- Kernel
- Device Tree

ensuring reliable system boot.

---
