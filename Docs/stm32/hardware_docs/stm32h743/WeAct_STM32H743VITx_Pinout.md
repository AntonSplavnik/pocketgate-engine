# WeAct STM32H743VITx Development Board - Complete Pinout

**Board:** WeAct Mini H743VITx
**Chip:** STM32H743VIT6 (LQFP100 package)
**Date:** 2026-02-25
**Purpose:** KiCad symbol creation reference

---

## Overview

- **Total Pins:** 88 (44 per side)
- **Package:** LQFP100
- **Voltage:** 3.3V logic, 5V tolerant on most pins
- **Power:** 3.3V regulated output, 5V input/output

---

## LEFT SIDE - Complete Pin Definitions (44 pins)

| KiCad Pin# | Pin Name | Type         | LQFP100     | AF1        | AF4          | AF5           | AF6       | AF7       | Notes            |
|------------|----------|--------------|-------------|------------|--------------|---------------|-----------|-----------|------------------|
| 1          | GND      | Power        | 10,26,49,98 | -          | -            | -             | -         | -         | Ground           |
| 2          | 5V       | Power In/Out | -           | -          | -            | -             | -         | -         | 5V input/output  |
| 3          | E1       | I/O          | 97          | LPTIM1_IN2 | -            | -             | -         | UART8_TX  |                  |
| 4          | E0       | I/O          | 96          | LPTIM1_ETR | -            | -             | -         | UART8_RX  |                  |
| 5          | B9       | I/O          | 95          | TIM4_CH4   | **I2C1_SDA** | -             | I2C4_SDA  | UART1_TX  | **Touch SDA**    |
| 6          | B8       | I/O          | 94          | TIM4_CH3   | **I2C1_SCL** | -             | I2C4_SCL  | UART4_RX  | **Touch SCL**    |
| 7          | B7       | I/O          | 93          | TIM4_CH2   | I2C1_SDA     | -             | I2C4_SDA  | UART1_RX  |                  |
| 8          | B6       | I/O          | 92          | TIM4_CH1   | I2C1_SCL     | -             | I2C4_SCL  | UART1_TX  |                  |
| 9          | B5       | I/O          | 91          | TIM3_CH2   | I2C1_SMBA    | **SPI3_MOSI** | I2C4_SMBA | SPI6_MOSI | **Display MOSI** |
| 10         | B4       | I/O          | 90          | TIM3_CH1   | -            | **SPI3_MISO** | -         | SPI2_NSS  | **Display MISO** |
| 11         | B3       | I/O          | 89          | TIM2_CH2   | -            | **SPI3_SCK**  | -         | SPI6_SCK  | **Display SCK**  |
| 12         | D7       | I/O          | 88          | -          | -            | SPI1_MOSI     | -         | UART2_CK  |                  |
| 13         | D6       | I/O          | 87          | -          | -            | SPI3_MOSI     | SAI1_SD_A | UART2_RX  |                  |
| 14         | D5       | I/O          | 86          | -          | -            | -             | SAI1_SD_B | UART2_TX  |                  |
| 15         | D4       | I/O          | 85          | -          | -            | -             | SAI3_FS_A | UART2_RTS |                  |
| 16         | D3       | I/O          | 84          | -          | -            | -             | SPI2_SCK  | UART2_CTS |                  |
| 17         | D2       | I/O          | 83          | TIM3_ETR   | -            | -             | -         | UART5_RX  |                  |
| 18         | D1       | I/O          | 82          | -          | -            | -             | SAI3_SD_A | UART4_TX  |                  |
| 19         | D0       | I/O          | 81          | -          | -            | -             | -         | UART4_RX  |                  |
| 20         | C12      | I/O          | 80          | -          | -            | SPI3_MOSI     | -         | UART5_TX  |                  |
| 21         | C11      | I/O          | 79          | -          | -            | SPI3_MISO     | -         | UART4_RX  |                  |
| 22         | C10      | I/O          | 78          | -          | -            | SPI3_SCK      | -         | UART4_TX  |                  |
| 23         | A15      | I/O          | 77          | TIM2_CH1   | -            | -             | SPI1_NSS  | UART4_TX  | JTDI             |
| 24         | A12      | I/O          | 71          | TIM1_ETR   | -            | -             | -         | UART4_TX  | USB_DP           |
| 25         | A11      | I/O          | 70          | TIM1_CH4   | -            | -             | -         | UART4_RX  | USB_DM           |
| 26         | A10      | I/O          | 69          | TIM1_CH3   | -            | -             | -         | -         | USB_ID           |
| 27         | A9       | I/O          | 68          | TIM1_CH2   | I2C3_SMBA    | -             | -         | UART1_TX  |                  |
| 28         | A8       | I/O          | 67          | TIM1_CH1   | **I2C3_SCL** | -             | -         | UART7_RX  | MCO1             |
| 29         | C9       | I/O          | 66          | TIM8_CH4   | I2C3_SDA     | -             | -         | UART5_CTS | MCO2             |
| 30         | C8       | I/O          | 65          | TIM8_CH3   | -            | -             | -         | UART5_RTS |                  |
| 31         | C7       | I/O          | 64          | TIM8_CH2   | -            | -             | -         | UART6_RX  |                  |
| 32         | C6       | I/O          | 63          | TIM8_CH1   | -            | -             | -         | UART6_TX  |                  |
| 33         | D15      | I/O          | 62          | TIM4_CH4   | -            | -             | -         | -         |                  |
| 34         | D14      | I/O          | 61          | TIM4_CH3   | -            | -             | -         | UART8_CTS |                  |
| 35         | D13      | I/O          | 60          | TIM4_CH2   | -            | -             | -         | UART8_RTS |                  |
| 36         | D12      | I/O          | 59          | TIM4_CH1   | I2C4_SCL     | -             | -         | UART8_TX  |                  |
| 37         | D11      | I/O          | 58          | -          | I2C4_SMBA    | -             | -         | UART8_RX  |                  |
| 38         | D10      | I/O          | 57          | -          | -            | -             | -         | -         |                  |
| 39         | D9       | I/O          | 56          | -          | -            | -             | -         | UART8_TX  |                  |
| 40         | D8       | I/O          | 55          | -          | -            | -             | -         | UART8_RX  |                  |
| 41         | B15      | I/O          | 54          | TIM1_CH3N  | -            | **SPI2_MOSI** | -         | UART4_CTS | RTC_REFIN        |
| 42         | B14      | I/O          | 53          | TIM1_CH2N  | -            | **SPI2_MISO** | -         | UART3_RTS |                  |
| 43         | B13      | I/O          | 52          | TIM1_CH1N  | -            | **SPI2_SCK**  | -         | UART3_CTS |                  |
| 44         | B12      | I/O          | 51          | TIM1_BKIN  | I2C2_SMBA    | **SPI2_NSS**  | -         | UART3_CK  |                  |

---

## RIGHT SIDE - Complete Pin Definitions (44 pins)

| KiCad Pin# | Pin Name | Type         | LQFP100     | AF1           | AF4          | AF5           | AF6         | AF7          | Notes              |
|------------|----------|--------------|-------------|---------------|--------------|---------------|-------------|--------------|--------------------|
| 45         | GND      | Power        | 10,26,49,98 | -             | -            | -             | -           | -            | Ground             |
| 46         | 3V3      | Power Out    | 11,50,74,99 | -             | -            | -             | -           | -            | 3.3V regulated     |
| 47         | E2       | I/O          | 1           | TRACE_CLK     | -            | SPI4_SCK      | SAI1_MCLK_A | -            |                    |
| 48         | E3       | I/O          | 2           | TRACE_D0      | -            | -             | SAI1_SD_B   | -            |                    |
| 49         | E4       | I/O          | 3           | TRACE_D1      | -            | -             | SAI1_FS_A   | -            |                    |
| 50         | E5       | I/O          | 4           | TRACE_D2      | -            | -             | SAI1_SCK_A  | -            |                    |
| 51         | E6       | I/O          | 5           | TRACE_D3      | -            | -             | SAI1_SD_A   | -            | **CTP_INT**        |
| 52         | VB       | Power In     | 6           | -             | -            | -             | -           | -            | Backup battery     |
| 53         | C13      | I/O          | 7           | -             | -            | -             | -           | -            | RTC wakeup         |
| 54         | NR       | Input        | 14          | -             | -            | -             | -           | -            | Reset (active low) |
| 55         | C0       | I/O          | 15          | -             | -            | -             | -           | SAI2_FS_B    | ADC1_IN10          |
| 56         | C1       | I/O          | 16          | -             | -            | SPI2_MOSI     | SAI1_SD_A   | -            | ADC1_IN11          |
| 57         | C2       | I/O          | 17          | -             | -            | SPI2_MISO     | -           | -            | ADC1_IN12          |
| 58         | C3       | I/O          | 18          | -             | -            | SPI2_MOSI     | -           | -            | ADC1_IN13          |
| 59         | GND      | Power        | 10,26,49,98 | -             | -            | -             | -           | -            | Ground             |
| 60         | V+       | Power In/Out | -           | -             | -            | -             | -           | -            | Power input        |
| 61         | A0       | I/O          | 22          | TIM2_CH1      | -            | -             | -           | UART4_TX     | ADC1_IN0           |
| 62         | A1       | I/O          | 23          | TIM2_CH2      | -            | -             | -           | UART4_RX     | ADC1_IN1           |
| 63         | A2       | I/O          | 24          | TIM2_CH3      | -            | -             | -           | UART2_TX     | ADC1_IN2           |
| 64         | A3       | I/O          | 25          | TIM2_CH4      | -            | -             | -           | UART2_RX     | ADC1_IN3           |
| 65         | A4       | I/O          | 28          | -             | -            | **SPI1_NSS**  | SPI3_NSS    | -            | ADC1_IN4           |
| 66         | A5       | I/O          | 29          | TIM2_CH1      | -            | **SPI1_SCK**  | -           | -            | ADC1_IN5           |
| 67         | A6       | I/O          | 30          | TIM1_BKIN     | -            | **SPI1_MISO** | -           | -            | ADC1_IN6           |
| 68         | A7       | I/O          | 31          | TIM1_CH1N     | -            | **SPI1_MOSI** | -           | -            | ADC1_IN7           |
| 69         | C4       | I/O          | 32          | -             | -            | -             | -           | -            | ADC1_IN14          |
| 70         | C5       | I/O          | 33          | -             | -            | -             | -           | -            | ADC1_IN15          |
| 71         | B0       | I/O          | 34          | TIM1_CH2N     | -            | -             | -           | UART4_CTS    |                    |
| 72         | B1       | I/O          | 35          | TIM1_CH3N     | -            | -             | -           | -            |                    |
| 73         | B2       | I/O          | 36          | RTC_OUT       | -            | -             | SAI1_SD_A   | -            |                    |
| 74         | E7       | I/O          | 37          | TIM1_ETR      | -            | -             | -           | UART7_RX     | **CTP_RST**        |
| 75         | E8       | I/O          | 38          | **TIM1_CH1N** | -            | -             | -           | UART7_TX     | **LED/Backlight**  |
| 76         | E9       | I/O          | 39          | TIM1_CH1      | -            | -             | -           | UART7_RTS    | **LCD_DC**         |
| 77         | E10      | I/O          | 40          | TIM1_CH2N     | -            | -             | -           | UART7_CTS    | **LCD_RST**        |
| 78         | E11      | I/O          | 41          | TIM1_CH2      | -            | **SPI4_NSS**  | SAI2_SD_B   | -            | **LCD_CS**         |
| 79         | E12      | I/O          | 42          | TIM1_CH3N     | -            | **SPI4_SCK**  | -           | -            |                    |
| 80         | E13      | I/O          | 43          | TIM1_CH3      | -            | **SPI4_MISO** | -           | -            |                    |
| 81         | E14      | I/O          | 44          | TIM1_CH4      | -            | **SPI4_MOSI** | -           | -            |                    |
| 82         | E15      | I/O          | 45          | TIM1_BKIN     | -            | -             | -           | -            |                    |
| 83         | B10      | I/O          | 46          | TIM2_CH3      | **I2C2_SCL** | -             | -           | **UART3_TX** |                    |
| 84         | B11      | I/O          | 47          | TIM2_CH4      | **I2C2_SDA** | -             | -           | **UART3_RX** |                    |
| 85         | 3V3      | Power Out    | 11,50,74,99 | -             | -            | -             | -           | -            | 3.3V regulated     |
| 86         | 5V       | Power In/Out | -           | -             | -            | -             | -           | -            | 5V input/USB       |
| 87         | GND      | Power        | 10,26,49,98 | -             | -            | -             | -           | -            | Ground             |
| 88         | GND      | Power        | 10,26,49,98 | -             | -            | -             | -           | -            | Ground             |

---

## Pin Type Legend

| Type      | Description |
|-----------|-------------|
| I/O       | Input/Output - bidirectional GPIO |
| Power In  | Power supply input |
| Power Out | Regulated power output |
| Power     | Ground or power pin |
| Input     | Input only (Reset, Boot) |

---

## Current Project Pin Usage

### Display (ST7796S) - SPI3
- **PB3** (Pin 11): SPI3_SCK
- **PB4** (Pin 10): SPI3_MISO
- **PB5** (Pin 9): SPI3_MOSI
- **PE9** (Pin 76): LCD_DC (Data/Command)
- **PE10** (Pin 77): LCD_RST (Reset)
- **PE11** (Pin 78): LCD_CS (Chip Select)
- **PE8** (Pin 75): LED (Backlight control)

### Touch Controller (FT6336U) - I2C1
- **PB8** (Pin 6): I2C1_SCL
- **PB9** (Pin 5): I2C1_SDA
- **PE6** (Pin 51): CTP_INT (Interrupt)
- **PE7** (Pin 74): CTP_RST (Reset)

### Power
- **GND** (Pins 1, 45, 59, 87, 88): Ground
- **3V3** (Pins 46, 85): 3.3V regulated output
- **5V** (Pins 2, 86): 5V input/output
- **V+** (Pin 60): Power input
- **VBAT** (Pin 52): Backup battery

---

## Notes

1. **Bold** functions indicate commonly used alternate functions
2. LQFP100 pin numbers refer to physical chip package pins
3. Most I/O pins are 5V tolerant (FT type)
4. ADC inputs are analog-capable pins
5. USB pins (A11, A12) require specific routing considerations
6. All alternate functions require GPIO configuration via AF registers

---

## References

- STM32H743VI Datasheet
- WeAct STM32H7XX Schematic V1.2
- STM32H7 Reference Manual (RM0433)

---

## KiCad Electrical Pin Type Summary

For proper ERC (Electrical Rules Check) in KiCad:

| Pin Type     | KiCad Type           | Pin Numbers                                                                   |
|--------------|----------------------|-------------------------------------------------------------------------------|
| GPIO (I/O)   | Bidirectional        | 3-44 (except 1,2), 47-54, 55-58, 61-84                                        |
| Power Output | Power output         | 46 (3V3), 85 (3V3)                                                            |
| Power Input  | Power input.         | 1 (GND), 2 (5V), 45 (GND), 52 (VBAT), 59 (GND), 60 (V+), 86 (5V), 87-88 (GND) |
| Input Only   | Input                | 54 (NR/NRST).                                                                 |

---

**Created:** 2026-02-25
**Updated:** 2026-02-25 (Corrected to 88 pins based on physical board)
**For:** KiCad symbol creation - WeAct STM32H743VITx module
