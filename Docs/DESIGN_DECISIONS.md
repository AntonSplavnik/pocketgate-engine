# TriggEngine Design Decisions

This document records major architectural and design decisions made during the development of TriggEngine. Each decision includes the context, alternatives considered, trade-offs, and rationale.

**Format:**
- **Decision**: What was decided
- **Context**: Why this decision was needed
- **Alternatives Considered**: Other options evaluated
- **Trade-offs**: Pros and cons
- **Rationale**: Why this choice was made
- **Status**: Active / Revisit / Deprecated
- **Date**: When decided

---

## Table of Contents

1. [Multi-Game Architecture (Cartridge System)](#1-multi-game-architecture-cartridge-system)
2. [C++11 Standard](#2-c11-standard)
3. [Display-Agnostic Architecture](#3-display-agnostic-architecture)
4. [Isometric Rendering Approach](#4-isometric-rendering-approach)
5. [Memory Allocation Strategy](#5-memory-allocation-strategy)

---

## 1. Multi-Game Architecture (Cartridge System)

**Status**: Active
**Date**: 2025-12-17

### Decision

Implement an **Engine + Data architecture** where:
- Core engine resides in Flash (fixed, compiled)
- Game data resides on SD Card (swappable, no compilation)
- Boot menu allows selecting games from SD card
- Games are data-driven (manifests, tilemaps, sprites, entities)

### Context

Need to support multiple game development and play without reflashing the Pico each time. Flash memory is read-only at runtime (only writable via USB bootloader or SWD programmer).

### Alternatives Considered

#### Option A: Engine + Data on SD (Chosen)
```
Flash (2MB):                SD Card (GB):
├── Engine code             ├── game1/
├── Renderer                ├── game2/
├── Physics                 └── game3/
└── Game loader
```

**Pros:**
- Multiple games without reflashing
- Fast iteration (edit data, copy to SD, test)
- Engine updates benefit all games
- User-friendly game selection menu
- 60 FPS performance (engine runs from Flash)

**Cons:**
- RAM budget constraint (~200KB for assets)
- Engine must be generic (not game-specific)
- 50-100ms load time per game/level
- Need well-designed data formats

#### Option B: Bootloader + Full Game Binaries
```
Flash:                      SD Card:
├── Bootloader (64KB)       ├── game1.bin
└── Game slot (1.9MB)       ├── game2.bin
                            └── game3.bin
```

**Pros:**
- Each game is full C++ binary
- No performance overhead
- Complete flexibility per game

**Cons:**
- Complex bootloader implementation
- Pico only has 264KB RAM (can't load full binary)
- Would need XIP from SD (slow, complex)
- Can't fit large games in RAM

#### Option C: Manual Reflashing per Game
Simple Python script to flash different builds.

**Pros:**
- Simple implementation
- Full control per game
- No runtime complexity

**Cons:**
- Requires USB connection every time
- Slow development workflow
- Not user-friendly
- Can't easily distribute multiple games

### Trade-offs

| Aspect | Impact |
|--------|--------|
| **Performance** | ✅ No impact - engine runs at 60 FPS from Flash, only loading is slower |
| **Development Speed** | ✅ Much faster - edit data files, no recompilation |
| **User Experience** | ✅ Console-like game menu, easy to add games |
| **Flexibility** | ⚠️ Limited to engine capabilities (data-driven only) |
| **Memory** | ⚠️ RAM budget ~200KB for active assets |
| **Complexity** | ⚠️ Need to design data formats and loader system |

### Rationale

**Performance**: Testing shows SD card read speeds (2-4 MB/s) only affect loading, not gameplay. Loading 100KB of game data takes ~40-60ms, which is acceptable during level transitions. The game loop runs entirely from Flash/RAM at full 60 FPS.

**Memory Budget**: 200KB RAM is sufficient for:
- Framebuffer: 40KB
- Tile cache: 40KB
- Sprite cache: 60KB
- Entities: 20KB
- Game state: 40KB

**Development Workflow**: This mirrors classic console architecture (NES, Game Boy, SNES) where firmware is fixed and games are swappable cartridges. It's a proven model.

**Scalability**: As engine improves, all games benefit. New games can be added without touching engine code.

### Implementation Notes

See detailed implementation in `Docs/memory/sd_card.md:350-620`.

**Game folder structure:**
```
SD:/games/
├── game1/
│   ├── manifest.dat   (metadata: name, version, asset counts)
│   ├── tilemap.dat    (level layout)
│   ├── sprites.dat    (RGB565 sprite pixel data)
│   └── entities.dat   (entity definitions)
├── game2/
└── game3/
```

**Data format approach**: Start with pure data-driven (Phase 1). Add scripting/bytecode interpreter later if needed (Phase 2+).

### Future Considerations

- **Scripting system**: May add Lua or custom bytecode VM for game logic in Phase 4+
- **Compression**: Consider LZ4 or RLE compression for assets if RAM becomes tight
- **Streaming**: Large levels could be streamed in chunks if needed
- **Save games**: Store on SD card per-game (e.g., `game1/saves/`)

---

## 2. C++11 Standard

**Status**: Active
**Date**: 2025-12-17 (Project Start)

### Decision

Use **C++11** as the maximum language standard. No C++14, C++17, or C++20 features.

### Context

Raspberry Pi Pico SDK officially supports C++11. Newer standards may have limited or untested support.

### Alternatives Considered

#### Option A: C++11 (Chosen)
**Pros:**
- Full Pico SDK support
- Well-tested on embedded systems
- Sufficient features: `auto`, `nullptr`, range-based `for`, lambdas, smart pointers
- Stable and predictable

**Cons:**
- Missing modern conveniences (structured bindings, `std::optional`, etc.)
- More verbose code in some cases

#### Option B: C++17
**Pros:**
- `std::optional`, `std::variant`
- Structured bindings
- `if constexpr`

**Cons:**
- Uncertain Pico SDK support
- May increase binary size
- Risk of compiler issues

#### Option C: C (not C++)
**Pros:**
- Smallest binaries
- Maximum control
- No overhead

**Cons:**
- No classes, templates, or RAII
- More verbose and error-prone
- Harder to maintain complex engine

### Rationale

C++11 provides the right balance of modern features and embedded system compatibility. Features like RAII, smart pointers, and lambdas significantly improve code quality without runtime overhead.

**Allowed C++11 features:**
- `auto` keyword
- `nullptr`
- Range-based `for` loops
- Lambdas
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Move semantics
- `constexpr`
- Strongly-typed enums (`enum class`)

**Optimization priorities:**
- Minimize heap allocations (prefer stack)
- Use `const` correctness
- Prefer compile-time constants (`constexpr`)
- Avoid exceptions (embedded best practice)

---

## 3. Display-Agnostic Architecture

**Status**: Active
**Date**: 2025-12-17 (Project Start)

### Decision

Design engine to support **future display upgrades** through abstraction:
- Never hardcode resolution values
- Use `SCREEN_WIDTH` / `SCREEN_HEIGHT` constants
- Abstract display driver interface
- Separate driver layer from engine logic

### Context

Current hardware: ST7735 128×160 display
Future hardware: ST7789 240×240 or ILI9341 320×240

Project goal is to build a console that can evolve with better displays.

### Alternatives Considered

#### Option A: Display-Agnostic (Chosen)
```cpp
// hardware_config.h
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

// Engine code uses constants
framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
```

**Pros:**
- Supports display upgrades (change driver + constants only)
- Clean separation of concerns
- Future-proof
- Asset system can support multiple resolutions

**Cons:**
- Slight abstraction overhead
- Can't use resolution-specific optimizations

#### Option B: Hardcode for ST7735
```cpp
// Hardcoded values
framebuffer[128 * 160];
```

**Pros:**
- Simpler code
- Can optimize for exact resolution

**Cons:**
- Display upgrade requires rewriting engine
- Not future-proof
- Harder to maintain

### Rationale

Hardware upgrades are explicitly planned (see CLAUDE.md:23-30). Abstracting the display adds minimal overhead but provides significant flexibility. Changing from 128×160 to 240×240 should only require:

1. Swap driver implementation (`st7735.cpp` → `st7789.cpp`)
2. Update constants in `hardware_config.h`
3. Rebuild

Engine code remains unchanged.

### Implementation Guidelines

**DO:**
```cpp
for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        framebuffer[y * SCREEN_WIDTH + x] = color;
    }
}
```

**DON'T:**
```cpp
for (int y = 0; y < 160; y++) {  // ❌ Hardcoded
    for (int x = 0; x < 128; x++) {  // ❌ Hardcoded
        framebuffer[y * 128 + x] = color;
    }
}
```

---

## 4. Isometric Rendering Approach

**Status**: Active
**Date**: 2025-12-17 (Project Start)

### Decision

Use **diamond-shaped isometric tiles** with the following specs:
- Tile size: 32×16 pixels
- Coordinate conversion: Standard isometric formulas
- Render order: Back-to-front (top-left to bottom-right)
- Depth sorting: Sort entities by `(x + y)` value

### Context

Need to render 3D-looking world on 2D display for game aesthetic and gameplay depth.

### Alternatives Considered

#### Option A: Diamond Isometric (Chosen)
```
Tile: 32×16 pixels (diamond shape)

screen_x = (world_x - world_y) * tile_width/2
screen_y = (world_x + world_y) * tile_height/2
```

**Pros:**
- Classic isometric look (Diablo, Age of Empires style)
- Natural depth perception
- Well-understood algorithms
- Tile-based optimization

**Cons:**
- More complex rendering than top-down
- Requires depth sorting for entities
- Camera scrolling needs careful handling

#### Option B: Top-Down 2D
```
screen_x = world_x * tile_width
screen_y = world_y * tile_height
```

**Pros:**
- Simplest rendering
- No depth sorting needed
- Easier collision detection

**Cons:**
- Less visual depth
- Looks flat/dated
- Limited perspective variety

#### Option C: Fake 3D (Mode 7)
**Pros:**
- Impressive visual effect
- Smooth scaling/rotation

**Cons:**
- Complex math (matrix transforms)
- High CPU usage on Pico
- Hard to integrate with tile-based gameplay

### Rationale

Diamond isometric provides the best balance of visual appeal and performance. The 32×16 tile size allows:
- ~10×10 visible tiles on 128×160 display
- 60 FPS rendering with culling
- Memory-efficient tile-based storage

**Performance target**: 10×10 tiles + 10-20 entities at 60 FPS is achievable with:
- Tile culling (only render visible tiles)
- Dirty rectangle tracking
- DMA for framebuffer transfer

See detailed implementation in `Docs/isometric/rendering.md` and `Docs/isometric/depth_sorting.md`.

---

## 5. Memory Allocation Strategy

**Status**: Active
**Date**: 2025-12-17 (Project Start)

### Decision

**Memory usage priorities:**

1. **Flash (2MB)**: Store engine code, display drivers, loader
2. **RAM (264KB)**: Allocate as follows:
   - Framebuffer: 40KB (128×160×2 bytes)
   - Tile cache: 40KB
   - Sprite cache: 60KB
   - Entity data: 20KB
   - Game state: 20KB
   - Stack/heap: 20KB
   - System overhead: ~64KB
3. **SD Card**: Store game data, music, user content

### Context

Pico has limited RAM (264KB total, ~200KB usable after system overhead). Must carefully budget memory to fit framebuffer + assets.

### Alternatives Considered

#### Option A: Single Framebuffer (Chosen)
```cpp
uint16_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];  // 40KB
```

**Pros:**
- Saves 40KB RAM
- Enough for assets

**Cons:**
- Potential screen tearing (mitigated by fast SPI)

#### Option B: Double Buffering
```cpp
uint16_t framebuffer[2][SCREEN_WIDTH * SCREEN_HEIGHT];  // 80KB
```

**Pros:**
- No tearing
- Smooth rendering

**Cons:**
- Uses 80KB (40% of RAM!)
- Less space for assets

### Rationale

Single framebuffer with 62.5 MHz SPI transfer is fast enough (~8ms to transfer 40KB). Screen tearing is minimal and acceptable for 60 FPS games. The 40KB saved allows for more sprites and tiles in RAM.

**Asset loading strategy:**
- Store all assets in Flash (read-only)
- Load active level assets to RAM
- Unload/reload between levels
- Stream large assets (music) from SD card

**Optimization techniques:**
- Use pointers to Flash data when possible
- Compress sprites if needed (LZ4, RLE)
- Tile-based rendering (only draw visible tiles)
- Entity pooling (pre-allocate, reuse)

---

## Future Decisions

### To Be Decided

- **Audio System**: PWM audio? I2S DAC? SD streaming?
- **Input Handling**: Direct GPIO polling vs interrupt-driven?
- **Asset Pipeline**: Manual conversion vs build-time tools?
- **Compression**: LZ4, RLE, or uncompressed assets?
- **Scripting**: Lua, custom bytecode, or pure data-driven?
- **Network**: Future ESP32 co-processor for multiplayer?

### Deferred

- **Save System**: Flash vs SD card storage (decide in Phase 4)
- **Audio Implementation**: Deferred until Phase 5+
- **Multiplayer**: Out of scope for MVP

---

## Decision Review Process

**When to revisit a decision:**
- Performance targets are not met
- New hardware capabilities become available
- User feedback indicates issues
- Better alternatives are discovered

**Review schedule:**
- After each major phase completion
- When hitting technical limitations
- Quarterly check-in on active decisions

---

## References

- **CLAUDE.md**: Project constraints and standards
- **INFO.md**: Development roadmap
- **Docs/memory/**: Memory architecture details
- **Docs/isometric/**: Rendering implementation
- **Docs/hardware/**: Hardware specifications
