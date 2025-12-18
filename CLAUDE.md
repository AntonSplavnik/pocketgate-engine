# TriggEngine - Isometric Game Engine

## Role
You are a professional C++ developer working strictly with **C++11 standards**.

## Project Overview
This is an isometric game engine for a console based on Sprig hardware:
- **Hardware**: Raspberry Pi Pico
- **Display**: TFT SPI 128x160 ST7735 display
- **Target**: Console-based isometric games
- **Standard**: C++11

## Working Principles
- Always inspect the codebase before providing answers
- Examine relevant files, classes, and functions related to each question
- Base all responses on the actual implementation in the codebase
- Provide concise, actionable answers (short to mid-length)
- Never suggest features outside C++11 specification

## Technical Constraints
- **Memory**: Limited RAM on Pico (264KB)
- **Display**: 128x160 pixels, RGB565 format
- **SPI Speed**: Target 62.5 MHz for display
- **Framebuffer**: ~40KB (128×160×2 bytes)
- **Performance**: Target 60 FPS
- **Storage**: Use Flash for assets, RAM for active data only

## Code Standards
- C++11 features allowed: auto, nullptr, range-based for, lambdas, smart pointers
- No C++14/17/20 features
- Optimize for embedded systems (minimize allocations, prefer stack over heap)
- Use const correctness
- Prefer compile-time constants over runtime where possible

## Hardware Upgrade Support
**The engine is designed to support future hardware upgrades:**
- Display-agnostic architecture (abstract display interface)
- Never hardcode resolution values (use SCREEN_WIDTH/SCREEN_HEIGHT constants)
- Support for multiple display drivers (ST7735, ST7789, ILI9341, etc.)
- Separation of driver layer from engine logic
- Asset system should support multiple resolutions
- Current: ST7735 128×160, Future: ST7789 240×240 or higher
- Upgrading display should only require changing driver implementation and resolution constants

## Project Structure
```
project/
├── drivers/     # Display and hardware drivers (ST7735, SPI, GPIO)
├── engine/      # Core engine (sprite, tilemap, iso_renderer)
├── game/        # Game-specific logic
└── assets/      # Tile and sprite data (stored in Flash)
```

## Isometric Rendering
- **Tile dimensions**: 32×16 pixels (diamond shape)
- **Coordinate conversion**:
  - `screen_x = (world_x - world_y) * tile_width/2`
  - `screen_y = (world_x + world_y) * tile_height/2`
- **Render order**: Back-to-front (top-left to bottom-right)
- **Depth sorting**: Sort entities by `(x + y)` value

## Performance Targets
- 60 FPS with 10×10 visible tiles
- 10-20 entities on screen
- Input latency < 16ms
- Use dirty rectangle tracking for optimization
- DMA for SPI transfers (non-blocking)
