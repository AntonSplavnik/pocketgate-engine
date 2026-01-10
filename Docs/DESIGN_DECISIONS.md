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
6. [Endianness: Little-Endian Native with Display-Time Conversion](#6-endianness-little-endian-native-with-display-time-conversion)

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

## 6. Endianness: Little-Endian Native with Display-Time Conversion

**Status**: Active
**Date**: 2026-01-10

### Decision

Store all framebuffer and sprite data in **little-endian (CPU-native) format** throughout the engine. Perform batch endianness conversion **only when sending to display** via SPI.

**Implementation:**
- Sprites stored as little-endian RGB565 in `.h` asset files
- Framebuffer operations use little-endian RGB565 natively
- `swap_endian()` converts entire framebuffer to big-endian before SPI send
- `swap_endian()` converts back to little-endian after SPI send (for next frame)

### Context

The Raspberry Pi Pico (ARM Cortex-M0+) is **little-endian**, while the ST7735 TFT display expects RGB565 data in **big-endian** format over SPI (MSB first). This creates an endianness mismatch requiring byte swapping somewhere in the rendering pipeline.

**Key constraints:**
- Display: Requires big-endian RGB565 over SPI
- CPU: Operates natively in little-endian
- Operations: ~5,000 pixel writes + ~500 alpha-blended pixels per frame
- Display send: 20,480 pixels (128×160) per frame

### Alternatives Considered

#### Option A: Little-Endian Native + Batch Swap (Chosen)

```cpp
// Sprite storage (little-endian)
const SpritePixel sprite_data[] = {
    {0xF800, 255},  // Native little-endian
    // ...
};

// Rendering (no swaps needed!)
back_buffer[i] = sprite.color;  // Direct write

// Alpha blending (simple bit operations)
uint8_t r = (color >> 11) & 0x1F;  // Clean extraction
uint8_t g = (color >> 5) & 0x3F;
uint8_t b = color & 0x1F;
// ... blend ...
result = (r << 11) | (g << 5) | b;  // Clean packing

// Display send (batch conversion)
void send_to_display() {
    swap_endian(front_buffer);  // Convert all 20,480 pixels
    send_data((uint8_t*)front_buffer, 40960);
    swap_endian(front_buffer);  // Convert back
}

void swap_endian(uint16_t* buffer) {
    for (size_t i = 0; i < 20480; i++) {
        buffer[i] = __builtin_bswap16(buffer[i]);
    }
}
```

**Pros:**
- ✅ All rendering operations use CPU-native format (fastest)
- ✅ Simple, clean bit operations for RGB extraction/packing
- ✅ No swaps during rendering (~5,500 operations per frame)
- ✅ Alpha blending straightforward: `(color >> 11) & 0x1F`
- ✅ Batch swap highly optimized (tight loop, compiler optimization)
- ✅ Industry-standard approach (lvgl, ugui, emWin all use this)
- ✅ Easier to understand and maintain

**Cons:**
- ⚠️ Requires 40,960 swaps per frame (20,480 before + after display send)
- ⚠️ Framebuffer must be swapped twice per frame

#### Option B: Big-Endian Storage + Per-Pixel Swap

```cpp
// Sprite storage (big-endian)
const SpritePixel sprite_data[] = {
    {0x00F8, 255},  // Big-endian (swapped at build time)
    // ...
};

// Rendering (requires swap on write)
back_buffer[i] = (color << 8) | (color >> 8);  // Swap on every write

// Alpha blending (complex byte manipulation)
uint8_t s_hi = color & 0xFF;        // Extract bytes
uint8_t s_lo = color >> 8;
uint8_t r = s_hi >> 3;              // Complex bit manipulation
uint8_t g = ((s_hi & 0x07) << 3) | (s_lo >> 5);  // Multi-operation
uint8_t b = s_lo & 0x1F;
// ... blend ...
uint8_t res_hi = (r << 3) | (g >> 3);
uint8_t res_lo = ((g & 0x07) << 5) | b;
result = res_hi | (res_lo << 8);

// Display send (no swap needed)
send_data((uint8_t*)front_buffer, 40960);  // Direct send
```

**Pros:**
- ✅ No swap needed when sending to display
- ✅ Direct SPI transfer

**Cons:**
- ❌ Swap on every pixel write (~5,000 per frame)
- ❌ Complex byte manipulation for alpha blending (~500 pixels/frame)
- ❌ Scattered swaps prevent compiler optimization
- ❌ Working against CPU's native byte order (slower)
- ❌ Code harder to read and maintain
- ❌ Non-standard approach for embedded graphics

#### Option C: Hybrid (Store Both Formats)

Store both little and big-endian versions of sprites.

**Pros:**
- No runtime conversion

**Cons:**
- ❌ Doubles sprite memory usage (critical constraint)
- ❌ Complex asset management
- ❌ Wasteful of limited Flash/RAM

### Trade-offs

| Aspect | Little-Endian Native | Big-Endian Storage |
|--------|---------------------|-------------------|
| **Rendering Performance** | ✅ Direct writes, no conversion | ⚠️ Swap every write (~5,000×/frame) |
| **Alpha Blending** | ✅ Simple bit shifts (5 ops) | ❌ Complex byte manipulation (13 ops) |
| **Code Clarity** | ✅ Standard RGB565 operations | ❌ Confusing byte-level code |
| **Display Send** | ❌ 40,960 swaps (1.15ms) | ✅ No conversion (0ms) |
| **Total Performance** | ❌ ~1.35ms/frame | ✅ ~0.34ms/frame |
| **Frame Budget Impact** | ⚠️ 6% of 16.67ms budget | ✅ 2% of 16.67ms budget |
| **Code Maintainability** | ✅ Simple, industry-standard | ❌ Error-prone, custom approach |
| **Future Features** | ✅ Direct pixel reads/effects | ❌ Un-swap needed for operations |
| **Debugging** | ✅ Colors appear correct in memory | ❌ Colors appear swapped in debugger |

### Rationale

**Performance Analysis:**

Let's be honest about the performance numbers:

**Big-Endian Storage (scattered swaps):**
- Pixel writes: ~5,000 swaps
- Alpha blending: ~1,000 swaps (500 pixels × 2 for un-swap/re-swap)
- Total: ~6,000 swaps at ~7 cycles each = **42,000 cycles = 0.34ms**
- Display send: 0ms (no conversion)

**Little-Endian Native (batch swaps):**
- Rendering: 0 swaps
- Display send: 40,960 swaps (before + after) at ~3.5 cycles = **143,360 cycles = 1.15ms**
- Alpha blending: Simpler operations save ~6,500 cycles = 0.05ms

**Total per frame:**
- Big-endian: ~0.34ms
- Little-endian: ~1.35ms

**Little-endian is ~1ms slower per frame.** This is the honest truth.

**Why Little-Endian is Still the Better Choice:**

The 1ms cost (~6% of 16.67ms frame budget) is **acceptable** because:

1. **Code Maintainability**: Simple bit operations vs complex byte manipulation
   - Fewer bugs (we encountered color inversion bugs during big-endian implementation)
   - Easier to understand and modify
   - Standard RGB565 documentation applies directly

2. **Flexibility for Future Features**:
   - Reading pixels for effects: Direct access vs un-swap every read
   - Color comparisons: `if ((color & 0xF800) > threshold)` works naturally
   - Palette operations: Simple bit manipulation
   - Color space conversions: Standard algorithms apply

3. **Alpha Blending Simplicity**: 5 operations vs 13 operations (60% reduction in complexity)

4. **Industry Standard**: LVGL, uGUI, emWin, TouchGFX all use CPU-native format
   - Proven approach in production embedded graphics
   - Well-understood optimization techniques
   - Future optimization paths (DMA-assisted swap, dirty rectangles)

5. **Frame Budget**: 1ms out of 16.67ms (6%) is negligible for 60 FPS target
   - Current rendering: ~4-5ms
   - Physics/logic: ~2-3ms
   - Display send: ~8ms (SPI transfer)
   - Available headroom: ~3ms
   - **1ms cost fits comfortably in budget**

6. **Optimization Opportunities**:
   - Dirty rectangle tracking: Only swap changed regions
   - DMA future: Hardware-assisted swap (if supported in later Pico versions)
   - Partial updates: Reduce swap area for UI elements

**The Trade-Off:**

We're trading 1ms of performance for significantly cleaner, more maintainable code. In embedded systems, this is often the right choice when the performance cost is acceptable within the frame budget.

**Code Quality:**

Alpha blending comparison:
```cpp
// Little-endian (clean)
uint8_t r = (color >> 11) & 0x1F;
result = (r << 11) | (g << 5) | b;

// Big-endian (complex)
uint8_t s_hi = color & 0xFF;
uint8_t r = s_hi >> 3;
uint8_t res_hi = (r << 3) | (g >> 3);
result = res_hi | (res_lo << 8);
```

Little-endian code is:
- Easier to understand
- Easier to debug
- Matches standard RGB565 documentation
- Less error-prone

**Industry Standard:**

All major embedded graphics libraries (LVGL, uGUI, emWin, TouchGFX) use CPU-native format internally and convert at display time. This is proven best practice.

### Implementation

**File: `engine/graphics/framebuffer.cpp`**

```cpp
// Batch endian conversion helper
void swap_endian(uint16_t* buffer) {
    size_t num_pixels = DISPLAY_HEIGHT * DISPLAY_WIDTH;
    for (size_t i = 0; i < num_pixels; i++) {
        buffer[i] = __builtin_bswap16(buffer[i]);
    }
}

// Display send with conversion
void Framebuffer::send_to_display() {
    swap_endian(front_buffer);  // Little → Big
    set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    send_data((uint8_t*)front_buffer, DISPLAY_WIDTH * DISPLAY_HEIGHT * 2);
    swap_endian(front_buffer);  // Big → Little (restore for next frame)
}

// Simple pixel write (no swap!)
void Framebuffer::set_pixel(uint16_t x, uint16_t y, uint16_t color) {
    back_buffer[y * DISPLAY_WIDTH + x] = color;
}

// Clean alpha blending (native operations)
void Framebuffer::draw_sprite_alpha(...) {
    // Extract RGB (simple bit shifts)
    uint8_t sr = (pixel.color >> 11) & 0x1F;
    uint8_t sg = (pixel.color >> 5) & 0x3F;
    uint8_t sb = pixel.color & 0x1F;

    // Blend...

    // Pack (simple bit shifts)
    back_buffer[i] = (r << 11) | (g << 5) | b;
}
```

**Sprite converter: `tools/sprite_to_cpp/sprite_to_cpp_alpha.py`**

```python
# Line 32: Use little-endian unpacking
rgb565 = struct.unpack('<H', data[offset:offset+2])[0]  # '<' = little-endian
```

### Future Considerations

- **DMA optimization**: Consider DMA byte-swapping if Pico DMA supports it (currently it doesn't)
- **Partial updates**: Dirty rectangle tracking to reduce swap area
- **Hardware upgrade**: If future display has native little-endian support, remove `swap_endian()` calls entirely

### Related Documentation

- `Docs/hardware/endianness.md`: Detailed endianness explanation
- `engine/graphics/framebuffer.cpp`: Implementation
- `tools/sprite_to_cpp_alpha.py`: Asset conversion

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
