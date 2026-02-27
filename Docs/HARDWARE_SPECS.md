# PocketGate - Hardware Specifications

Hardware reference for the PocketGate handheld console.
All specs are based on actual components in use.

---

## System Overview

| Role             | Component                                             |
|------------------|-------------------------------------------------------|
| MCU              | WeAct Studio STM32H743VIT6 Core Board                 |
| Display          | 4.0" Capacitive SPI Module ST7796S + FT6336U          |
| Audio            | WeAct I2S Speaker Module (PCM5100A DAC + NS4150B amp) |
| Speakers         | 2× 8Ω 2W 28mm                                         |
| WiFi             | ESP8266 D1 Mini                                       |
| Battery          | 2× 18650 2000mAh Li-ion (1S2P)                        |
| Power Management | SW6106 PD 18W BMS Module                              |
| External RAM     | PSRAM64H 64Mbit SOP-8                                 |

---

## MCU — WeAct Studio STM32H743VIT6

| Parameter        | Value                       |
|------------------|-----------------------------|
| Chip             | STM32H743VIT6               |
| Core             | ARM Cortex-M7               |
| Clock            | Up to 480 MHz               |
| Flash (internal) | 2 MB                        |
| Package          | LQFP100                     |
| Logic voltage    | 3.3V (most I/O 5V tolerant) |
| USB              | USB FS/HS (PA11/PA12)       |
| Board supply     | 3.3V out, 5V in/out         |

### Internal SRAM Regions

| Region                 | Size         | Bus               | Notes                       |
|------------------------|--------------|-------------------|-----------------------------|
| DTCM                   | 128 KB       | CPU only (no DMA) | Fastest access, stack       |
| AXI SRAM               | 512 KB       | CPU + MDMA        | Large buffers               |
| SRAM1 + SRAM2 + SRAM3  | 288 KB total | DMA1/DMA2         | SPI/I2S DMA buffers         |
| SRAM4                  | 64 KB        | BDMA              | Low-power peripherals       |
| **Total internal**     | **~1 MB**    |                   | Fragmented across domains   |

**Note:** Framebuffers (2× ~300 KB at 320×480 RGB565) cannot fit in internal SRAM — external PSRAM required.

### Key Peripherals Used

| Peripheral                   | Function                  |
|------------------------------|---------------------------|
| SPI3                         | Display (ST7796S)         |
| I2C1                         | Touch controller (FT6336U)|
| I2S (SAI or SPI in I2S mode) | Audio to PCM5100A         |
| UART                         | ESP8266 communication     |
| QSPI (OCTOSPI)               | External PSRAM64H         |
| TIM1_CH1N                    | Display backlight PWM     |
| EXTI6                        | Touch interrupt           |

---

## Display — 4.0" ST7796S Capacitive SPI Module

| Parameter                  | Value                                              |
|----------------------------|----------------------------------------------------|
| Display controller         | ST7796S                                            |
| Touch controller           | FT6336U (capacitive)                               |
| Resolution                 | 320 × 480 pixels                                   |
| Color depth                | RGB565 (16-bit, 65K colors)                        |
| Display interface          | SPI (4-wire)                                       |
| Touch interface            | I2C (address 0x38)                                 |
| Supply voltage             | 3.3V or 5V (**5V recommended**)                    |
| Backlight                  | LED, PWM-dimmable                                  |
| Framebuffer size           | 320 × 480 × 2 bytes = **300 KB** (RGB565, no alpha)|
| Framebuffer size (+ alpha) | 320 × 480 × 3 bytes = **450 KB** (RGB565 + 8-bit alpha) |

### Display Pin Assignment (current wiring)

| Signal    | STM32 Pin | Peripheral       |
|-----------|-----------|------------------|
| SPI MOSI  | PB5       | SPI3_MOSI        |
| SPI MISO  | PB4       | SPI3_MISO        |
| SPI SCK   | PB3       | SPI3_SCK         |
| LCD_CS    | PE11      | GPIO             |
| LCD_RST   | PE10      | GPIO             |
| LCD_DC    | PE9       | GPIO             |
| Backlight | PE8       | TIM1_CH1N / GPIO |
| CTP_SDA   | PB9       | I2C1_SDA         |
| CTP_SCL   | PB8       | I2C1_SCL         |
| CTP_INT   | PE6       | EXTI6            |
| CTP_RST   | PE7       | GPIO             |
| SD_CS     | PE5       | GPIO             |

---

## Audio — WeAct I2S Speaker Module (PCM5100A + NS4150B)

### PCM5100A — I2S DAC

| Parameter      | Value                                    |
|----------------|------------------------------------------|
| Chip           | PCM5100A (Texas Instruments)             |
| Function       | Stereo audio DAC                         |
| Interface      | I2S (standard 3-wire: BCK, LRCK, DIN)    |
| Resolution     | Up to 32-bit                             |
| Sample rate    | Up to 384 kHz                            |
| Supply voltage | 3.3V (analog + digital)                  |
| Output         | Differential analog → NS4150B input      |

### PCM5100A Supply Pins (all 3.3V)

| Pin    | Voltage | Powers                              | Required caps          |
|--------|---------|-------------------------------------|------------------------|
| AVDD   | 3.3V    | DAC, Reference, PLL, Line Driver    | 10µF + 100nF ceramic   |
| CPVDD  | 3.3V    | Charge Pump, Oscillator             | 10µF + 100nF ceramic   |
| DVDD   | 3.3V    | Digital IO (+ internal 1.8V LDO)    | 1µF + 100nF ceramic    |
| CPVNEG | output  | Charge pump negative rail (−3.3V)   | 1µF ceramic (filter)   |
| LDOO   | output  | Digital Core, DSPs (1.8V generated) | 1µF ceramic (filter)   |

### NS4150B — Class D Amplifier

| Parameter      | Value                                                                              |
|----------------|------------------------------------------------------------------------------------|
| Chip           | NS4150B (NanJing Micro One)                                                        |
| Function       | Mono Class D power amplifier                                                       |
| Output power   | 2.8W into 4Ω (per channel)                                                         |
| Configuration  | BTL (Bridge-Tied Load)                                                             |
| Supply voltage | 3.3V – 5V (runs at 3.3V when module is 3.3V powered; full 2.8W requires 5V)        |

### Module Summary

| Parameter         | Value                                                        |
|-------------------|--------------------------------------------------------------|
| Channels          | Stereo (2× NS4150B)                                          |
| Max output        | 2× 2.8W into 4Ω                                              |
| Speaker impedance | 4Ω (matched to 8Ω speakers via parallel pair, TBD)           |
| Supply            | **5V** (module has onboard ME6231A33M3G LDO → 3.3V for PCM5100A; NS4150B runs on 5V for full power) |

---

## Speakers — 28mm Full-Range

| Parameter       | Value                                                         |
|-----------------|---------------------------------------------------------------|
| Quantity        | 2                                                             |
| Impedance       | 8Ω each                                                       |
| Power rating    | 2W each                                                       |
| Diameter        | 28mm                                                          |
| **Connection**  | TBD — 8Ω per channel, or 2× in parallel → 4Ω per channel      |

**Note:** NS4150B is rated for 4Ω loads. Running a single 8Ω speaker per channel is safe but reduces power output. Two speakers per channel in parallel (4Ω) uses full amplifier rating but doubles speaker count to 4 total.

---

## WiFi — ESP8266 D1 Mini

| Parameter          | Value                                                        |
|--------------------|--------------------------------------------------------------|
| Chip               | ESP8266MOD                                                   |
| Core               | Tensilica L106 32-bit                                        |
| Clock              | 80 MHz (up to 160 MHz)                                       |
| Flash              | 4 MB                                                         |
| WiFi standard      | 802.11 b/g/n (2.4 GHz)                                       |
| Interface to STM32 | UART (AT command firmware or custom)                         |
| Operating voltage  | 3.3V DC                                                      |
| Input voltage      | 5V (via USB or 5V pin, on-board regulator steps down)        |

---

## External RAM — PSRAM64H

| Parameter     | Value                                    |
|---------------|------------------------------------------|
| Chip                    | ESP-PSRAM64H (Espressif)                          |
| Capacity                | 64 Mbit = **8 MB**                                |
| Package                 | SOP-8                                             |
| Supply voltage          | 3.3V                                              |
| Interface               | QSPI (Quad SPI, 4 data lines)                     |
| Max clock               | 133 MHz (no page boundary crossing)               |
| Max clock (page cross)  | 84 MHz                                            |
| Active current          | 40 mA max (from datasheet)                        |
| Standby current         | 200 µA max (from datasheet)                       |
| Connected via           | STM32H743 OCTOSPI / QUADSPI peripheral            |

**Usage:** Primary framebuffer storage. Two framebuffers (450 KB each) = 900 KB — fits within 8 MB with room for asset caching.

---

## Battery — 2× 18650 Li-ion 2000mAh (1S2P)

| Parameter             | Value                              |
|-----------------------|------------------------------------|
| Cell count            | 2                                  |
| Configuration         | 1S2P (2 cells in parallel)         |
| Chemistry             | Li-ion                             |
| Nominal voltage       | 3.7V                               |
| Max charge voltage    | 4.2V                               |
| Min discharge voltage | ~3.0V (cutoff)                     |
| Capacity per cell     | 2000 mAh                           |
| Total capacity        | **4000 mAh** at 3.7V = **~14.8 Wh** |
| Managed by            | SW6106 BMS                         |

---

## Power Management — SW6106 Module

| Parameter                    | Value                                              |
|------------------------------|----------------------------------------------------|
| Chip                         | SW6106                                             |
| USB input                    | USB Type-C, USB PD 18W                             |
| Input voltage range          | 5V – 20V (PD negotiated)                           |
| Charging current             | Up to 4A (1S Li-ion)                               |
| BMS                          | 1S cell: overcharge, overdischarge, overcurrent    |
| USB output                   | 5V DC output (for system power and USB-A charging) |
| Cell configuration supported | 1S (single cell or parallel cells at 1S voltage)   |

### Power Architecture

```
2× 18650 Li-ion (1S2P, 3.7V nominal, 4000mAh)
                │
            SW6106
            ├── USB-C IN  (5–20V PD charging)
            ├── BAT       (charges cells at up to 4A)
            └── 5V OUT    (boost converter, max 3A)
                    │
                    ├── [C1] 100µF electrolytic  ← bulk, SW6106 output
                    ├── [C2] 100nF ceramic X7R   ← HF bypass
                    │
        ────────────┴────────────5V rail──────────────────────────┐
        │                                                         │
 WeAct STM32 board                                                │
 (5V pin → AMS1117)                          ┌────────────────────┴────────────────┐
        │                                    │                    │                │
    3.3V rail                          Display ST7796S    ESP8266 D1 Mini    Audio module
    │                                 (5V recommended)   (5V → LDO → 3.3V)  (5V input)
    ├── [C3] 10µF ceramic X5R  ← bulk, 3.3V rail                                   │
    ├── [C4] 100nF ceramic X7R ← HF bypass                                   ME6231A33M3G
    │                                                                        (onboard LDO)
    └─→ PSRAM64H (bare chip)                                                        │
         [C5] 100nF ceramic X7R  ← required                              VDD33 (3.3V)
         [C6] 10µF ceramic X5R   ← required                                         │
                                                                       ┌────────────┴────────────┐
                                                                       │                         │
                                                                   PCM5100A DAC              NS4150B amps
                                                                 (3.3V regulated)            (5V direct)
                                                                 AVDD/CPVDD/DVDD              VCC = 5V
                                                                                             (full power)
```

### Capacitor Reference List

| Ref | Value  | Type          | Location                    | Purpose                               |
|-----|--------|---------------|-----------------------------|---------------------------------------|
| C1  | 100µF  | Electrolytic  | 5V rail at SW6106 output    | Bulk reservoir, transient handling    |
| C2  | 100nF  | Ceramic X7R   | 5V rail at SW6106 output    | HF bypass                             |
| C3  | 10µF   | Ceramic X5R   | 3.3V rail at WeAct 3V3 pin  | Bulk, stabilise 3.3V rail             |
| C4  | 100nF  | Ceramic X7R   | 3.3V rail at WeAct 3V3 pin  | HF bypass                             |
| C5  | 100nF  | Ceramic X7R   | PSRAM64H VDD, close to pin  | Per-chip bypass (**required** — bare chip, standard practice, no datasheet value) |
| C6  | 10µF   | Ceramic X5R   | PSRAM64H VDD                | Bulk for PSRAM (**required** — bare chip, standard practice, no datasheet value)  |

**Priority for prototype:**
- **Must add now:** C1–C6 (5V/3.3V rail bulk + PSRAM bare chip)
- **Already onboard audio module:** C7–C14 (PCM5100A supply caps — included on WeAct module PCB)
- **Already handled:** STM32 board, D1 Mini, display module, audio module all have their own onboard caps

### 5V Rail Consumers

| Component          | From 5V      | Notes                       |
|--------------------|--------------|-----------------------------|
| WeAct STM32 board  | ~200–380mA   | AMS1117 feeds 3.3V rail     |
| Display ST7796S    | ~40–80mA     | 5V recommended              |
| ESP8266 D1 Mini    | ~170–320mA   | WiFi TX peak                |
| Audio module       | ~30–60mA     | ME6231A33M3G LDO + NS4150B amps (idle to moderate) |
| **Total peak**     | **~1.1–1.9A**| Within SW6106 3A limit      |

### 3.3V Rail Consumers (via AMS1117, max 1A)

| Component      | Current       | Notes                        |
|----------------|---------------|------------------------------|
| STM32H743VIT6  | ~130–280mA    | 480MHz, all peripherals      |
| PSRAM64H       | 40mA max      | Active QSPI reads/writes (datasheet) |
| **Total typical** | **~170–320mA** | Comfortable for AMS1117 (audio module has its own 3.3V regulator) |

**Note — prototype vs custom PCB:**
- Prototype: AMS1117 LDO is adequate (~66% efficient, generates some heat)
- Custom PCB: replace AMS1117 with a 3.3V buck converter (~90% efficient) for better battery life

---

## Memory Budget Summary

| Region                     | Size   | Assigned use                        |
|----------------------------|--------|-------------------------------------|
| DTCM (internal)            | 128 KB | Stack, critical variables, fast code|
| AXI SRAM (internal)        | 512 KB | Working buffers, single-buffer testing |
| SRAM1/2/3 (internal)       | 288 KB | DMA buffers (SPI TX, I2S)           |
| SRAM4 (internal)           | 64 KB  | Low-power / BDMA peripherals        |
| PSRAM64H (external)        | 8 MB   | Framebuffers (×2), asset cache      |
| Internal Flash             | 2 MB   | Engine code, renderer               |
| QSPI Flash (external, TBD) | TBD    | Game assets, sprites, tilemaps      |

---

## Interfaces Summary

| Interface        | Peripheral   | Connected to                  |
|------------------|--------------|-------------------------------|
| SPI3             | Display data | ST7796S                       |
| I2C1             | Touch        | FT6336U                       |
| I2S / SAI        | Audio data   | PCM5100A                      |
| UART (TBD)       | WiFi AT      | ESP8266                       |
| OCTOSPI/QUADSPI  | External RAM | PSRAM64H                      |
| USB FS           | DFU / Debug  | USB-C (via STM32 directly)    |

---

*Last updated: 2026-02-26*
