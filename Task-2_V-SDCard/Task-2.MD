# Lab 2: Creating a Virtual SD Card for Embedded Linux Development

## 1. Create a 1 GiB Virtual Disk Image

### Task
Create the image and explain the command.

### Command
```bash
dd if=/dev/zero of=sd.img bs=1M count=1024 status=progress
```

### Explanation
- **dd**: Data Duplicator utility used for low-level copying.
- **if=/dev/zero**: Input file. A special file that provides an infinite stream of null bytes (^@).
- **of=sd.img**: Output file. The name of the virtual disk image being created.
- **bs=1M**: Block size. Reads/writes 1 Megabyte per operation.
- **count=1024**: Number of blocks to copy (1M × 1024 = 1024MB = 1GiB).
- **status=progress**: Displays progress during execution.

---

## 2. DOS/MBR vs. GPT Partition Schemes

### Task
Define the differences.

| Feature | DOS/MBR (Master Boot Record) | GPT (GUID Partition Table) |
|----------|------------------------------|----------------------------|
| Standard | Legacy BIOS standard | Modern UEFI standard |
| Max Disk Size | Up to 2 TB | Zettabytes (practically unlimited) |
| Partitions | Max 4 primary (or 3 primary + 1 extended) | Theoretically unlimited (Windows caps at 128) |
| Redundancy | No redundancy (stored at start only) | Redundant headers at start and end |

---

## 3. File Systems & Usage

### FAT16
- **Usage**: Small boot partitions in embedded systems.
- **Pros**: Simple, universally supported by BIOS/UEFI.
- **Cons**: Limited size (2–4GB max), inefficient on large drives.

### FAT32
- **Usage**: Removable media (USB drives, SD cards).
- **Pros**: Cross-platform compatibility (Windows, Linux, macOS).
- **Cons**: 4GB maximum file size, no permissions or security model.

### EXT4
- **Usage**: Standard Linux root filesystem (rootfs).
- **Pros**: Linux permissions, ownership, journaling, large file support.
- **Cons**: Not natively supported by Windows.

---

## 4. Partitioning the Virtual Disk

### Task
Partition `sd.img` into two primary partitions:
- Partition 1: 200MB, Bootable, FAT16 (Type 6 or e)
- Partition 2: Remaining space, Linux/EXT4 (Type 83)

### Command
```bash
cfdisk sd.img
```

(Alternative: `fdisk sd.img` for non-graphical mode)

### Steps in cfdisk
1. Select **DOS** label type.
2. Create **New → 200M → Primary → Bootable**.
3. Set **Type → FAT16**.
4. Select free space → **New → Extended → (use remaining space)**.
5. Set **Type → Linux**.
6. **Write → type "yes" → Quit**.

---

## 5. Loop Devices

### Definition
A loop device is a pseudo block device that makes a file behave like a physical storage device. It allows mounting disk image files (e.g., `sd.img`) as if they were real disks.

### Commands
Attach and scan partitions
```bash
sudo losetup -f --show --partscan sd.img
```
List active loop devices
```bash
losetup -a
```
Detach loop device
```bash
sudo losetup -d /dev/loopX
```

---

## 6. Check Loop Device Limit

```bash
cat /sys/module/loop/parameters/max_loop
```

---

## 7. Expand Loop Device Number

### Methods
- **Boot Argument (GRUB)**:
  Add `max_loop=64` to kernel parameters.

- **If loop is a module**:
```bash
sudo modprobe loop max_loop=64
```

- **Manual device creation**:
Use `mknod` to create additional loop device nodes.

---

## 8. Attach Virtual Disk

```bash
sudo losetup -f --show --partscan sd.img
```

- **-f**: Find first unused loop device.
- **--show**: Print allocated loop device (e.g., `/dev/loop0`).
- **--partscan**: Scan partition table and create `/dev/loop0p1`, `/dev/loop0p2`.

---

## 9. Format Partitions

### Boot Partition (FAT16)
```bash
sudo mkfs.vfat -F 16 -n boot /dev/loop0p1
```

- `-F 16`: Force FAT16.
- `-n boot`: Set volume label.

### Rootfs Partition (EXT4)
```bash
sudo mkfs.ext4 -L rootfs /dev/loop0p2
```

- `-L rootfs`: Set volume label.

---

## 10. Mount vs. Unmount

- **mount**: Attaches a storage device to a directory (mount point) in the Linux filesystem tree.
- **umount**: Safely detaches the device and flushes cached data to prevent corruption.

---

## 11. Block vs. Character Devices

| Feature | Block Device | Character Device |
|----------|--------------|-----------------|
| Data Access | Block-based, buffered | Byte stream, unbuffered |
| Access Mode | Random access | Sequential access |
| Examples | Hard drives (sda), loop devices | Keyboard, mouse, ttyS0 |

---

## 12. Mount Points & Mounting

### Create Mount Points
```bash
mkdir -p mount/boot
mkdir -p mount/rootfs
```

### Mount Partitions
```bash
sudo mount /dev/loop0p1 mount/boot
sudo mount /dev/loop0p2 mount/rootfs
```

### Verification
```bash
lsblk
```


