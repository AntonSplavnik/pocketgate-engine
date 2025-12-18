# RAM (264KB)

## What is RAM?

RAM (Random Access Memory) is **volatile memory** - like your computer's RAM. It loses all data when powered off but is extremely fast for both reading and writing.

**Physical location:** Built into the RP2040 chip (SRAM)

---

## Characteristics

| Property | Value |
|----------|-------|
| **Size** | 264KB (270,336 bytes) |
| **Read speed** | <1 nanosecond per byte (instant) |
| **Write speed** | <1 nanosecond per byte (instant) |
| **Write cycles** | Unlimited |
| **Access method** | Direct CPU access |
| **Volatile?** | Yes - loses data when powered off |

---

## What RAM Contains

- **Framebuffer** (40KB) - Updated 60 times per second
- **Variables** - player position, score, health
- **Entity array** - Active enemies, items
- **Stack** - Function calls, local variables
- **Heap** - Dynamically allocated memory (if using malloc)

---

## RAM is Limited - Use Carefully!

**264KB seems like a lot, but it goes fast:**

```
RAM (264KB total):
├─ Framebuffer:         40KB   (mandatory for display)
├─ Pico SDK:            20KB   (system overhead)
├─ Stack:               8KB    (function calls)
├─ Current tilemap:     4KB    (64×64 map)
├─ Entities (20):       2KB    (game objects)
├─ Variables:           1KB    (player, score, etc)
├─ Audio buffer:        8KB    (if using sound)
└─ REMAINING:           ~181KB free

Total used: ~83KB / 264KB
```

**Always monitor RAM usage during development!**

---

## When to Use RAM

### ✅ Use RAM For:

**1. Framebuffer (Mandatory)**
```cpp
// MUST be in RAM - written every frame
uint16_t framebuffer[128*160];  // 40KB
```

**2. Game State (Changes Frequently)**
```cpp
// Player data
int player_x = 64, player_y = 80;
int health = 100;
int score = 0;

// Entities
struct Entity {
    int16_t x, y;
    uint8_t type;
    uint8_t state;
};
Entity enemies[20];  // 2KB
```

**3. Current Level Data (If Modifying)**
```cpp
// Copy from Flash if you need to modify tiles at runtime
uint8_t current_tilemap[64*64];  // 4KB

void loadLevel(int level) {
    // Copy from Flash to RAM
    memcpy(current_tilemap, level_data[level], 4096);

    // Now you can modify it
    current_tilemap[10] = TILE_GRASS;  // Change tile
}
```

**4. Temporary Buffers**
```cpp
// Temporary calculations
uint16_t temp_sprite[32*32];  // 2KB
```

### ❌ Don't Use RAM For:

**Static assets (use Flash instead):**
```cpp
// ❌ BAD: Wastes 2KB of RAM
uint16_t player_sprite[32*32];  // Stored in RAM

// ✅ GOOD: 0 bytes RAM, read from Flash
const uint16_t player_sprite[32*32] = { /* data */ };
```

---

## Speed Advantage

RAM is **3× faster than Flash** for reading:

```cpp
// Flash read: ~3µs per 1KB
const uint16_t* flash_tile = grass_tile;  // In Flash
blitTile(framebuffer, flash_tile, x, y);  // 3µs

// RAM read: ~1µs per 1KB
uint16_t ram_tile[512];  // In RAM
blitTile(framebuffer, ram_tile, x, y);    // 1µs
```

**But:** Flash is fast enough for 60 FPS, so only copy to RAM if you need to modify data.

---

## Memory Layout

**How RAM is organized:**

```
0x20000000  ┌─────────────────────────┐
            │  Stack (grows down)     │  8KB
            ├─────────────────────────┤
            │  Global variables       │
            │  - framebuffer (40KB)   │
            │  - tilemap (4KB)        │
            │  - entities (2KB)       │
            ├─────────────────────────┤
            │  Heap (grows up)        │  (if using malloc)
            ├─────────────────────────┤
            │  Unused RAM             │  ~181KB
0x20042000  └─────────────────────────┘
```

---

## Optimizing RAM Usage

### 1. Use Smaller Data Types
```cpp
// ❌ Wastes RAM
int player_x;  // 4 bytes on RP2040

// ✅ Better
int16_t player_x;  // 2 bytes (range: -32768 to 32767)
uint8_t tile_id;   // 1 byte (range: 0 to 255)
```

### 2. Pack Structs
```cpp
// ❌ 8 bytes per entity (padding)
struct Entity {
    int x, y;        // 4 + 4 = 8 bytes
    uint8_t type;    // 1 byte
    uint8_t state;   // 1 byte
};  // Total: 12 bytes (compiler adds 2 bytes padding)

// ✅ 6 bytes per entity
struct Entity {
    int16_t x, y;    // 2 + 2 = 4 bytes
    uint8_t type;    // 1 byte
    uint8_t state;   // 1 byte
} __attribute__((packed));  // Total: 6 bytes

// Savings: 20 entities = 120 bytes saved
```

### 3. Don't Copy from Flash Unless Needed
```cpp
// ❌ Wastes 4KB RAM
uint8_t tilemap[64*64];
memcpy(tilemap, level1_flash, 4096);

// ✅ Read directly from Flash (0 bytes RAM)
const uint8_t* tilemap = level1_flash;
uint8_t tile = tilemap[x + y*64];  // Fast read
```

### 4. Reuse Buffers
```cpp
// ❌ Multiple buffers
uint16_t buffer1[1024];
uint16_t buffer2[1024];
uint16_t buffer3[1024];  // 6KB total

// ✅ Reuse one buffer
uint16_t shared_buffer[1024];  // 2KB total
```

---

## Monitoring RAM Usage

**Check during compilation:**

After `make`, look for:
```
Memory region         Used Size  Region Size  %age Used
           FLASH:      147456 B         2 MB      7.04%
             RAM:       83968 B       264 KB     31.05%
```

**If RAM usage > 80%, optimize!**

---

## Common RAM Mistakes

### Mistake 1: Putting Assets in RAM
```cpp
// ❌ BAD: 40KB of RAM wasted!
uint16_t all_sprites[20][32*32];  // 20 sprites × 2KB = 40KB

// ✅ GOOD: 0 bytes RAM
const uint16_t all_sprites[20][32*32] = { /* in Flash */ };
```

### Mistake 2: Large Local Arrays
```cpp
void someFunction() {
    // ❌ BAD: 40KB on stack!
    uint16_t temp[128*160];  // Stack overflow!
}

// ✅ GOOD: Use global or static
static uint16_t temp[128*160];  // Or make it global
```

### Mistake 3: Not Freeing malloc
```cpp
void gameLoop() {
    while(1) {
        // ❌ BAD: Memory leak!
        uint8_t* data = malloc(1024);
        // Forgot to free()
    }
}

// ✅ GOOD: Always free
uint8_t* data = malloc(1024);
// Use data...
free(data);
```

---

## Key Takeaways

✅ **Use RAM for:** Framebuffer, changing variables, active game state
✅ **RAM is fast:** <1ns per byte (3× faster than Flash)
⚠️ **RAM is limited:** Only 264KB - use carefully
❌ **Don't waste RAM:** Store static assets in Flash
🔍 **Monitor usage:** Check build output, aim for <80%

**Best practice:** Use RAM only for data that changes at runtime.
