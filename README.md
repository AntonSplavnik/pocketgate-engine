# PocketGate Engine

A custom isometric game engine built from scratch in C++ for **PocketGate** — a handheld gaming console powered by the STM32H743 microcontroller.

<!-- TODO: Add a photo of the prototype hardware here -->
<!-- ![PocketGate Prototype](docs/images/prototype.jpg) -->

## What is PocketGate?

PocketGate is an open-source handheld gaming console built around the STM32H743 ARM Cortex-M7 microcontroller. The engine drives a 4-inch 480x320 TFT display, rendering sprite-based isometric worlds at 60 FPS — all on a microcontroller with ~1 MB of RAM.

Think classic isometric games like Fallout 1 or GBA-era RPGs, running on custom hardware you can hold in your hand.

The project spans the full stack: from PCB design and bare-metal drivers to a double-buffered rendering pipeline with fixed-point math, isometric coordinate systems, and an asset conversion toolchain.

## Features

**Rendering**
- Double-buffered framebuffer with pointer swapping
- RGB565 pixel format (16-bit color)
- Sprite rendering with per-pixel alpha blending
- Bresenham line drawing and diamond tile outlines
- Isometric world-to-screen coordinate conversion
- Camera/viewport system with offset tracking

**Engine**
- Q16.16 fixed-point arithmetic (no floating-point dependency)
- Movement system with boundary checks
- Entity Component System (in progress)
- DMA-accelerated display transfers

**Platform Support**
- **STM32H743** — target hardware with 4" ST7796S SPI display
- **Desktop (GLFW/OpenGL)** — development build for fast iteration

**Tooling**
- PNG to binary sprite converter (RGB565 + alpha)
- Sprite to C++ header generator (for Flash embedding)
- Sprite file inspector and alpha channel analyzer

## Hardware

| Component | Part |
|-----------|------|
| MCU | STM32H743VIT6 (Cortex-M7, 480 MHz) |
| Display | 4.0" ST7796S TFT, 480x320, SPI |
| Touch | FT6336U capacitive (I2C) |
| Audio | PCM5100A I2S DAC + NS4150B Class D amp |
| External RAM | PSRAM64H 8 MB (QSPI) |
| WiFi | ESP8266 D1 Mini (UART) |
| Power | SW6106 USB-C PD 18W BMS |
| Battery | 2x 18650 Li-ion, 4000 mAh (1S2P) |

<!-- TODO: Add photo of the components/wiring here -->
<!-- ![Hardware Components](docs/images/components.jpg) -->

A custom PCB is currently in development using KiCad — the next step after the breadboard prototype is validated.

## Architecture

```
PocketGate Engine
├── engine/                 Core engine (platform-independent)
│   ├── graphics/           Double-buffered framebuffer, sprite blitting
│   ├── isometric/          World/screen coordinate conversion, camera
│   └── math/               Q16.16 fixed-point arithmetic
│
├── platforms/
│   ├── stm32/              STM32H743 drivers (ST7796S, FT6336U, DMA)
│   └── desktop/            GLFW/OpenGL development build
│
├── game/                   Game logic (entity system, WIP)
├── assets/                 Sprites, tilesets, asset packs
└── tools/                  Asset conversion pipeline
```

The engine core is fully platform-independent. The same `framebuffer.cpp` and rendering code runs identically on the STM32 and the desktop build — only the display output layer differs.

### Rendering Pipeline

```
World Space (Fixed_q16)          Screen Space (pixels)         Display
       │                                │                        │
  Isometric projection ──►  Camera offset applied ──►  Framebuffer ──► SPI/DMA (STM32)
  world_to_screen()         back buffer render          swap()         or
                                                                  glTexImage2D (Desktop)
```

### Memory Layout (STM32H743)

```
Internal (~1 MB)                    External
├── DTCM 128 KB  → Stack            ├── PSRAM 8 MB → 2x framebuffers
├── AXI  512 KB  → Working buffers  └── QSPI Flash → Assets
├── SRAM 288 KB  → DMA buffers
└── SRAM4 64 KB  → Low-power
```

## Building (Desktop)

The desktop build lets you develop and test the engine without hardware. It renders the framebuffer to a GLFW window using OpenGL.

### Prerequisites

**macOS:**
```bash
brew install glfw
```

**Linux:**
```bash
sudo apt install libglfw3-dev libgl1-mesa-dev
```

### Build and Run

```bash
# Clone the repository
git clone https://github.com/AntonSplavnik/pocketgate-engine.git
cd pocketgate-engine

# Build
make -f platforms/desktop/Makefile.desktop

# Build and run
make -f platforms/desktop/Makefile.desktop run
```

### Controls

| Key | Action |
|-----|--------|
| W | Move up |
| A | Move left |
| S | Move down |
| D | Move right |
| ESC | Quit |

## Asset Pipeline

The engine includes tools for converting standard image files into optimized sprite data:

```bash
# Convert PNG to binary sprite (RGB565 + transparency)
./tools/png_to_sprite/png_to_sprite_alpha character.png

# Generate C++ header for Flash embedding
python tools/sprite_to_cpp/sprite_to_cpp_alpha.py character.sprite

# Inspect sprite file
./tools/inspecter/inspect_sprite character.sprite
```

Sprite format: 4-byte header (width + height as `uint16_t`) followed by raw RGB565 pixel data. Magenta (`0xF81F`) is used as the transparency key.

## Roadmap

### Software

- **Entity System** — component-based entity architecture for game objects
- **Tile Rendering** — filled isometric tiles with texture mapping
- **Collision System** — tile-based walkability and entity-to-entity checks
- **Depth Sorting** — back-to-front rendering with proper z-ordering
- **Viewport Culling** — render only visible tiles for performance
- **Animation** — sprite sheet frame management
- **Touch Input** — FT6336U capacitive touch integration
- **Audio** — I2S playback through the PCM5100A DAC
- **WiFi** — ESP8266 communication for multiplayer or updates

### Hardware

- **Prototype Assembly** — wire all components and validate on breadboard
- **Custom PCB** — KiCad schematic and board layout (in progress)
- **Enclosure** — 3D-printed or manufactured case design

## Project History

This project evolved through several hardware iterations — starting with a Raspberry Pi Pico and a small 128x160 ST7735 display, then migrating to the STM32H743 with a larger 480x320 screen as the scope and ambitions grew. The engine was restructured for multi-platform support along the way, and the Pico platform code remains in the repository.

## License

The **engine software** is open source under the [MIT License](LICENSE).

**Hardware designs** (KiCad schematics, PCB layouts) are proprietary and not covered by the software license.
