# QSPI (Quad Serial Peripheral Interface)

**What is QSPI?**

QSPI is an enhanced version of SPI that uses 4 data lines instead of 1. Think of it like upgrading from a single-lane road to a 4-lane highway - same speed limit, but 4x more traffic can flow.

**How it differs from SPI:**
- SPI: 1 data line in, 1 data line out (2 total)
- QSPI: 4 bidirectional data lines
- Result: 4x faster data transfer at the same clock speed

---

## Pin Comparison

### Standard SPI (4 pins)

```
MCU                     Device
 │                        │
 ├── CLK  ───────────────→│  Clock (timing)
 ├── MOSI ───────────────→│  Master Out, Slave In (data to device)
 ├── MISO ←───────────────│  Master In, Slave Out (data from device)
 └── CS   ───────────────→│  Chip Select (enable device)
```

### QSPI (6 pins)

```
MCU                     Device
 │                        │
 ├── CLK  ───────────────→│  Clock (timing)
 ├── IO0  ←──────────────→│  Data line 0 (bidirectional)
 ├── IO1  ←──────────────→│  Data line 1 (bidirectional)
 ├── IO2  ←──────────────→│  Data line 2 (bidirectional)
 ├── IO3  ←──────────────→│  Data line 3 (bidirectional)
 └── CS   ───────────────→│  Chip Select (enable device)
```

**Key difference:** QSPI data lines are bidirectional - they switch between input and output as needed.

---

## Data Transfer Comparison

### SPI: 1 bit per clock

```
Clock:  ⎍  ⎍  ⎍  ⎍  ⎍  ⎍  ⎍  ⎍     8 clock cycles
MOSI:   b7 b6 b5 b4 b3 b2 b1 b0    = 1 byte transferred
        ─────────────────────────
        8 clocks = 8 bits = 1 byte
```

### QSPI: 4 bits per clock

```
Clock:  ⎍     ⎍        2 clock cycles
IO0:    b0    b4   ╮
IO1:    b1    b5   │   = 1 byte transferred
IO2:    b2    b6   │
IO3:    b3    b7   ╯
        ─────────────
        2 clocks = 8 bits = 1 byte
```

**Same clock speed, 4x the throughput.**

---

## Speed Comparison

| Interface | Data Lines | Bits/Clock | @ 100MHz Clock |
|-----------|------------|------------|----------------|
| SPI       | 1          | 1          | 12.5 MB/s      |
| Dual SPI  | 2          | 2          | 25 MB/s        |
| **QSPI**  | 4          | 4          | **50 MB/s**    |
| Octal SPI | 8          | 8          | 100 MB/s       |

---

## QSPI Communication Phases

A typical QSPI transaction has multiple phases, each can use different modes:

```
┌──────────┬──────────┬──────────┬──────────┐
│ Command  │ Address  │  Dummy   │   Data   │
│ (1-bit)  │ (4-bit)  │ (cycles) │  (4-bit) │
└──────────┴──────────┴──────────┴──────────┘
     │          │          │          │
   8 bits    24 bits    0-31      N bytes
   1-wire    4-wire    cycles    4-wire
```

### Phase Details

1. **Command Phase**: Send instruction byte (usually 1-bit mode for compatibility)
2. **Address Phase**: Send memory address (can use 4-bit mode)
3. **Dummy Phase**: Wait cycles for device to prepare data (reads only)
4. **Data Phase**: Transfer actual data (4-bit mode)

---

## Common Use Cases

| Device Type | Why QSPI? |
|-------------|-----------|
| Flash Memory | Fast code execution, asset loading |
| PSRAM | Framebuffers, large data arrays |
| High-speed ADC | Fast sample collection |

**Not typically used for:**
- Displays (SPI is fast enough, simpler wiring)
- Sensors (low data rate, SPI sufficient)
- SD Cards (use SDIO for speed, or SPI for simplicity)

---

## Memory-Mapped Mode

QSPI peripherals often support **memory-mapped mode** - the external memory appears as part of the MCU's address space:

```
Without memory mapping:
    uint8_t data;
    qspi_read(0x1000, &data, 1);  // Explicit read function

With memory mapping:
    uint8_t* psram = (uint8_t*)0x90000000;
    uint8_t data = psram[0x1000];  // Direct access like internal RAM
```

**Benefits:**
- Use pointers directly to external memory
- DMA can access external memory
- Compiler can optimize access patterns
- Code reads more naturally

---

## QSPI on STM32H7

The STM32H7 series has a QUADSPI peripheral with two banks, allowing connection of two QSPI devices.

### Pin Mapping (STM32H743)

**Bank 1:**
| Function | Pin | Alternate Function |
|----------|-----|-------------------|
| CLK | PB2 | AF9 |
| NCS | PB6 | AF10 |
| IO0 | PD11 | AF9 |
| IO1 | PD12 | AF9 |
| IO2 | PE2 | AF9 |
| IO3 | PD13 | AF9 |

**Bank 2:**
| Function | Pin | Alternate Function |
|----------|-----|-------------------|
| CLK | PB2 | AF9 (shared) |
| NCS | PC11 | AF9 |
| IO0 | PE7 | AF10 |
| IO1 | PE8 | AF10 |
| IO2 | PE9 | AF10 |
| IO3 | PE10 | AF10 |

### Memory Addresses

| Bank | Address Range |
|------|---------------|
| Bank 1 | 0x90000000 - 0x9FFFFFFF |
| Bank 2 | 0x70000000 - 0x7FFFFFFF |

---

## Code Examples (STM32 HAL)

### Basic QSPI Initialization

```cpp
QSPI_HandleTypeDef hqspi;

void QSPI_Init(void) {
    hqspi.Instance = QUADSPI;
    hqspi.Init.ClockPrescaler = 1;           // Divide by 2
    hqspi.Init.FifoThreshold = 4;
    hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    hqspi.Init.FlashSize = 22;               // 2^(22+1) = 8MB
    hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
    hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
    hqspi.Init.FlashID = QSPI_FLASH_ID_1;
    hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

    HAL_QSPI_Init(&hqspi);
}
```

### Reading from QSPI PSRAM

```cpp
HAL_StatusTypeDef QSPI_Read(uint32_t address, uint8_t* buffer, uint32_t size) {
    QSPI_CommandTypeDef cmd = {0};

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = 0xEB;                  // Fast Read Quad I/O
    cmd.AddressMode = QSPI_ADDRESS_4_LINES;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = address;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.DummyCycles = 6;
    cmd.NbData = size;

    HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
    return HAL_QSPI_Receive(&hqspi, buffer, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}
```

### Writing to QSPI PSRAM

```cpp
HAL_StatusTypeDef QSPI_Write(uint32_t address, uint8_t* data, uint32_t size) {
    QSPI_CommandTypeDef cmd = {0};

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = 0x38;                  // Quad Write
    cmd.AddressMode = QSPI_ADDRESS_4_LINES;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = address;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.DummyCycles = 0;
    cmd.NbData = size;

    HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
    return HAL_QSPI_Transmit(&hqspi, data, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}
```

### Enabling Memory-Mapped Mode

```cpp
void QSPI_EnableMemoryMapped(void) {
    QSPI_CommandTypeDef cmd = {0};
    QSPI_MemoryMappedTypeDef memmap = {0};

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = 0xEB;                  // Fast Read Quad I/O
    cmd.AddressMode = QSPI_ADDRESS_4_LINES;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.DummyCycles = 6;

    memmap.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    HAL_QSPI_MemoryMapped(&hqspi, &cmd, &memmap);

    // Now access PSRAM like normal memory:
    // uint8_t* psram = (uint8_t*)0x90000000;
    // psram[0] = 0x42;
}
```

---

## PSRAM-Specific Commands

Common commands for QSPI PSRAM (ESP-PSRAM64H, APS6404L):

| Command | Code | Description |
|---------|------|-------------|
| Read | 0x03 | Standard read (1-bit) |
| Fast Read | 0x0B | Fast read with dummy cycles |
| Fast Read Quad | 0xEB | Quad read (4-bit) |
| Write | 0x02 | Standard write (1-bit) |
| Quad Write | 0x38 | Quad write (4-bit) |
| Enter QPI | 0x35 | Switch to full quad mode |
| Exit QPI | 0xF5 | Return to SPI mode |
| Reset Enable | 0x66 | Prepare for reset |
| Reset | 0x99 | Reset device |
| Read ID | 0x9F | Read manufacturer ID |

---

## Hardware Considerations

### Decoupling Capacitor

Always place a 100nF ceramic capacitor close to the PSRAM chip between VCC and GND:

```
        PSRAM
    ┌───────────┐
VCC─┤8        1├─CS
    │           │
    │           │
GND─┤4        5├─IO0
    └───────────┘
        │   │
      ┌─┴───┴─┐
      │ 100nF │  ← Within 5mm of chip
      └───────┘
```

**Why?** High-speed switching causes voltage fluctuations. The capacitor provides local charge storage and filters noise.

### Signal Integrity

At high speeds (100MHz+), consider:
- Keep traces short and equal length
- Avoid sharp corners in PCB traces
- Use ground plane under signal traces
- Add series termination resistors (22-33 ohm) if needed

---

## Troubleshooting

| Symptom | Possible Cause |
|---------|----------------|
| No response | Wrong pins, CS not toggling, chip not powered |
| Garbage data | Clock too fast, missing dummy cycles |
| Intermittent errors | Missing decoupling capacitor, signal integrity |
| Works then fails | Overheating, voltage droop under load |
| Only 1-bit mode works | QPI mode not enabled on chip |

---

## Summary

| Feature | SPI | QSPI |
|---------|-----|------|
| Data lines | 1 in + 1 out | 4 bidirectional |
| Speed multiplier | 1x | 4x |
| Pin count | 4 | 6 |
| Typical use | Displays, sensors | Flash, PSRAM |
| Memory mapping | Rare | Common |
| Complexity | Simple | Moderate |

**QSPI is the go-to interface for high-bandwidth external memory on modern MCUs.**
