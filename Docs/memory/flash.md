# Flash Memory (2MB)

## What is Flash?

Flash memory is **non-volatile storage** - like an SSD or hard drive in a computer. It keeps data when powered off and stores your compiled program and game assets.

**Physical location:** External chip on the Pico board

---

## Characteristics

| Property | Value |
|----------|-------|
| **Size** | 2MB (2,097,152 bytes) |
| **Read speed** | ~1-3 microseconds per byte |
| **Write speed** | Very slow (milliseconds) |
| **Write cycles** | ~100,000 times (limited) |
| **Access method** | Direct (memory-mapped, XIP mode) |
| **Volatile?** | No - keeps data when powered off |

---

## What Flash Contains

- Your compiled `.uf2` program code
- All `const` arrays (sprites, tiles, level data)
- Read-only game assets
- Engine code

---

## How to Store Data in Flash

### Using const Arrays (Recommended)

```cpp
// This data is automatically placed in Flash by the compiler
const uint16_t grass_tile[32*16] = {
    0xF800, 0x07E0, 0x001F, ...  // RGB565 pixel data
};

const uint8_t level1_tilemap[64*64] = {
    1, 1, 2, 3, 1, ...  // Tile IDs
};

// At runtime, CPU reads directly from Flash:
uint16_t color = grass_tile[0];  // Fast read, 0 bytes of RAM used!
```

### Why Use Flash for Assets?

✅ **Fast read speed** - 1-3µs per byte (suitable for 60 FPS)
✅ **Large capacity** - 2MB fits 100+ levels + all sprites
✅ **No RAM usage** - CPU reads directly, saves precious RAM
✅ **Direct access** - No file system overhead like SD card

---

## XIP Mode (Execute In Place)

The RP2040 uses **XIP (Execute In Place)** - the CPU can run code and read data directly from Flash without copying to RAM first.

**How it works:**
```
CPU needs sprite data
  ↓
CPU requests address 0x10000000
  ↓
Flash chip responds immediately (memory-mapped)
  ↓
CPU uses the data (no copying needed)
```

This is why Flash is fast enough for real-time game rendering!

---

## Writing to Flash at Runtime (Advanced)

⚠️ **Not recommended for beginners** - Flash writing has serious limitations:

### The Problem:
```cpp
#include "hardware/flash.h"

// ❌ Dangerous and slow!
void saveToDisk() {
    // Must erase entire 4KB sector first
    flash_range_erase(SAVE_OFFSET, 4096);  // Takes milliseconds!

    // Write new data
    flash_range_program(SAVE_OFFSET, data, size);

    // During erase/write:
    // - All interrupts disabled
    // - Display updates stop
    // - Game freezes
}
```

### When Flash Writing is Acceptable:
✅ Save games (happens rarely, user expects delay)
✅ High score tables
✅ Configuration settings

### When NOT to Write:
❌ During gameplay (causes frame drops)
❌ Frequently (limited write cycles)
❌ Multiple small writes (must erase 4KB sectors)

---

## Flash Memory Budget

**Typical game layout:**

```
Flash (2MB total):
├─ Engine code:         100KB
├─ Pico SDK:            50KB
├─ Sprites (20):        40KB  (20 × 2KB each)
├─ Tiles (20):          20KB  (20 × 1KB each)
├─ Levels (100):        1.1MB (100 × 11KB each)
├─ Fonts:               10KB
├─ Save data area:      64KB  (reserved for writes)
└─ REMAINING:           ~616KB free

Total used: ~1.4MB / 2MB
```

---

## Performance at 60 FPS

**Reading sprites from Flash is fast enough:**

```cpp
// Drawing 10 tiles per frame
void renderFrame() {
    for(int i = 0; i < 10; i++) {
        // Read tile from Flash: ~3µs per tile
        const uint16_t* tile = getTileFromFlash(i);
        blitTile(framebuffer, tile, x, y);
    }
}

// Total read time: 10 × 3µs = 30µs
// Frame budget: 16,667µs (60 FPS)
// Flash reads use only 0.2% of frame time!
```

**This is why we store sprites in Flash, not SD card.**

---

## Key Takeaways

✅ **Use Flash for:** Sprites, tiles, levels, engine code
✅ **Reads are fast:** 1-3µs per byte (60 FPS safe)
❌ **Writes are slow:** Milliseconds (avoid during gameplay)
✅ **Large capacity:** 2MB fits entire game
✅ **Zero RAM cost:** CPU reads directly via XIP

**Best practice:** Store all static game assets as `const` arrays in Flash.
