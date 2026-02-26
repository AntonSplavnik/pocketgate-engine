# DFU (Device Firmware Upgrade) Guide

## What is DFU?

**DFU** is a USB protocol for flashing firmware without a dedicated programmer.

STM32 chips have a **built-in bootloader** in ROM that supports DFU. This allows flashing via USB cable only.

```
┌──────────┐       USB       ┌─────────────────┐
│    PC    │ ◄─────────────► │     STM32       │
│ dfu-util │   DFU Protocol  │  (Bootloader)   │
└──────────┘                 └─────────────────┘
```

---

## DFU vs ST-Link

| Method | Hardware | Speed | Debugging |
|--------|----------|-------|-----------|
| DFU | USB cable only | Slower | No |
| ST-Link | $10-25 programmer | Faster | Yes |

---

## Enter DFU Mode

### Method 1: BOOT0 Button
1. Hold **BOOT0** button
2. Press **RESET** (or plug USB)
3. Release BOOT0

### Method 2: Already Running
If firmware supports it, can enter DFU via software command.

### Verify DFU Mode
```bash
dfu-util -l
```

Expected output:
```
Found DFU: [0483:df11] ... "Internal Flash" ...
```

---

## Install dfu-util

```bash
# macOS
brew install dfu-util

# Ubuntu/Debian
sudo apt install dfu-util

# Windows
# Download from http://dfu-util.sourceforge.net/
```

---

## Commands Reference

### List Devices
```bash
dfu-util -l
```

### Backup Firmware (Read)
```bash
dfu-util -a 0 -s <address>:<size> -U <output_file>
```

Example (STM32H743, 2MB flash):
```bash
dfu-util -a 0 -s 0x08000000:0x200000 -U backup.bin
```

### Flash Firmware (Write)
```bash
dfu-util -a 0 -s <address>:leave -D <input_file>
```

Example:
```bash
dfu-util -a 0 -s 0x08000000:leave -D firmware.bin
```

### Erase Flash
```bash
dfu-util -a 0 -s :mass-erase:force -D /dev/null
```

---

## Common Flags

| Flag | Meaning |
|------|---------|
| `-l` | List DFU devices |
| `-a <alt>` | Alt setting (0 = internal flash) |
| `-s <addr>` | Start address and modifiers |
| `-U <file>` | Upload (read FROM device) |
| `-D <file>` | Download (write TO device) |
| `-R` | Reset after operation |

### Address Modifiers

| Modifier | Effect |
|----------|--------|
| `:leave` | Exit DFU mode after flashing |
| `:mass-erase` | Erase entire flash |
| `:force` | Force operation |

---

## STM32 Flash Addresses

| Chip | Flash Start | Size |
|------|-------------|------|
| STM32H743 | 0x08000000 | 2MB (0x200000) |
| STM32F411 | 0x08000000 | 512KB (0x80000) |
| STM32F103 | 0x08000000 | 64-128KB |

---

## Troubleshooting

### Device Not Found
- Check USB connection
- Verify DFU mode (BOOT0 + RESET)
- Try different USB port/cable

### Permission Denied (Linux)
```bash
sudo dfu-util -l
# Or add udev rules for permanent fix
```

### Upload/Download Errors
- Try smaller transfer size
- Reset board and re-enter DFU mode
- Check if flash is read-protected

---

## PlatformIO Integration

In `platformio.ini`:
```ini
upload_protocol = dfu
```

Then:
```bash
pio run -t upload
```

PlatformIO calls dfu-util automatically.

---

## Quick Reference

```bash
# Check connection
dfu-util -l

# Backup 2MB flash
dfu-util -a 0 -s 0x08000000:0x200000 -U backup.bin

# Flash firmware
dfu-util -a 0 -s 0x08000000:leave -D firmware.bin

# Restore backup
dfu-util -a 0 -s 0x08000000:leave -D backup.bin
```
