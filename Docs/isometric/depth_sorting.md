# Depth Sorting (Entity Rendering Order)

When rendering entities (players, enemies, items) in an isometric game, you must draw them in the correct order to avoid visual artifacts where objects appear in front of things they should be behind.

---

## The Problem

Unlike tiles (which are always at the same depth), entities can move around and overlap each other.

```
❌ BAD: Wrong render order

    🧍 Player (drawn first)
   ╱ ╲
  🌳 Tree (drawn second, covers player)

Player should be in FRONT of tree, but tree was drawn last!
```

```
✅ GOOD: Correct render order

   🌳 Tree (drawn first)
   ╱ ╲
  🧍 Player (drawn second, appears in front)

Tree drawn first, then player appears in front correctly!
```

---

## The Rule: Sort by Depth

**Depth formula for isometric:**
```cpp
depth = world_x + world_y
```

**Entities further "back" in the world have smaller (x + y) values.**
**Entities further "forward" have larger (x + y) values.**

```
     (0,0)           depth = 0
        ↓
     (1,0)           depth = 1
    ╱   ╲
 (0,1)   (2,0)       depth = 1 and 2
    ╲   ╱
     (1,1)           depth = 2
        ↓
     (2,1)           depth = 3
        ↓
     (3,1)           depth = 4
```

**Rule:** Render entities with **smaller depth first**, larger depth last.

---

## Implementation

### Step 1: Calculate Depth for Each Entity

```cpp
struct Entity {
    int16_t world_x, world_y;  // Position in world
    uint8_t type;              // Player, enemy, tree, etc.
    const uint16_t* sprite;    // Pointer to sprite in Flash

    // Calculated depth for sorting
    int depth() const {
        return world_x + world_y;
    }
};
```

---

### Step 2: Sort Entities by Depth

```cpp
#include <algorithm>  // For std::sort (C++11)

Entity entities[20];
int num_entities = 10;

void renderEntities() {
    // Sort entities by depth (back to front)
    std::sort(entities, entities + num_entities,
        [](const Entity& a, const Entity& b) {
            return a.depth() < b.depth();  // Smaller depth first
        });

    // Render in sorted order
    for (int i = 0; i < num_entities; i++) {
        renderEntity(&entities[i]);
    }
}
```

**Without C++11 lambdas (manual sort):**
```cpp
// Bubble sort (simple, works for 10-20 entities)
void sortEntitiesByDepth() {
    for (int i = 0; i < num_entities - 1; i++) {
        for (int j = 0; j < num_entities - i - 1; j++) {
            if (entities[j].depth() > entities[j+1].depth()) {
                // Swap
                Entity temp = entities[j];
                entities[j] = entities[j+1];
                entities[j+1] = temp;
            }
        }
    }
}
```

---

### Step 3: Render Sorted Entities

```cpp
void renderEntity(const Entity* e) {
    // Convert world coordinates to screen coordinates
    int screen_x = (e->world_x - e->world_y) * 16 - camera_x;
    int screen_y = (e->world_x + e->world_y) * 8 - camera_y;

    // Draw sprite from Flash
    blitSprite(framebuffer, e->sprite, screen_x, screen_y);
}

void renderEntities() {
    // Sort by depth
    sortEntitiesByDepth();

    // Render back-to-front
    for (int i = 0; i < num_entities; i++) {
        renderEntity(&entities[i]);
    }
}
```

---

## Complete Rendering Pipeline

**Correct order for entire frame:**

```cpp
void renderFrame() {
    // 1. Clear framebuffer
    memset(framebuffer, 0, sizeof(framebuffer));

    // 2. Render tiles (back-to-front automatically)
    renderIsometricMap();

    // 3. Sort and render entities
    renderEntities();

    // 4. Render UI (always on top)
    renderUI();

    // 5. Send to display
    sendFramebuffer();
}
```

---

## Handling Ties (Same Depth)

When two entities have the same depth (same x + y value), use a secondary sort:

```cpp
// Sort by depth, then by y coordinate
std::sort(entities, entities + num_entities,
    [](const Entity& a, const Entity& b) {
        if (a.depth() == b.depth()) {
            return a.world_y < b.world_y;  // Tie-breaker
        }
        return a.depth() < b.depth();
    });
```

---

## Performance Considerations

### Sorting Cost

**For 10-20 entities:**
- Bubble sort: ~200 comparisons
- Quick sort (std::sort): ~40 comparisons

**Time cost:**
- Bubble sort: ~5µs
- std::sort: ~2µs

**Negligible compared to 16.67ms frame budget!**

---

### Optimization: Only Sort When Entities Move

```cpp
bool entities_moved = false;

void moveEntity(Entity* e, int dx, int dy) {
    e->world_x += dx;
    e->world_y += dy;
    entities_moved = true;  // Mark that sorting is needed
}

void renderEntities() {
    // Only sort if an entity moved
    if (entities_moved) {
        sortEntitiesByDepth();
        entities_moved = false;
    }

    // Render
    for (int i = 0; i < num_entities; i++) {
        renderEntity(&entities[i]);
    }
}
```

---

## Tall Sprites (Multi-Tile Entities)

For sprites taller than one tile (like trees, buildings):

```cpp
struct Entity {
    int16_t world_x, world_y;  // Base position
    uint8_t height;            // Height in tiles

    int depth() const {
        // Use bottom of sprite for depth
        return world_x + world_y + height;
    }
};
```

**Example:**
```cpp
// Tree at (5, 5), 3 tiles tall
Entity tree = { 5, 5, 3, ... };

// Depth = 5 + 5 + 3 = 13
// Will be drawn behind entities at (7, 7) with depth = 14
```

---

## Entities vs Tiles

**Render order within the same frame:**

```cpp
void renderFrame() {
    // For each tile row (back to front)
    for (int y = 0; y < MAP_HEIGHT; y++) {
        // Draw tiles for this row
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (tileVisible(x, y)) {
                renderTile(x, y);
            }
        }

        // Draw entities on this row
        for (int i = 0; i < num_entities; i++) {
            if (entities[i].world_y == y) {
                renderEntity(&entities[i]);
            }
        }
    }
}
```

**This interleaves tiles and entities perfectly!**

---

## Example: Complete Entity System

```cpp
#define MAX_ENTITIES 20

struct Entity {
    int16_t world_x, world_y;
    uint8_t type;
    const uint16_t* sprite;
    bool active;

    int depth() const {
        return world_x + world_y;
    }
};

Entity entities[MAX_ENTITIES];
int num_entities = 0;

void addEntity(int x, int y, uint8_t type, const uint16_t* sprite) {
    if (num_entities < MAX_ENTITIES) {
        entities[num_entities++] = { x, y, type, sprite, true };
    }
}

void sortEntitiesByDepth() {
    // Bubble sort (simple, fast enough for 20 entities)
    for (int i = 0; i < num_entities - 1; i++) {
        for (int j = 0; j < num_entities - i - 1; j++) {
            if (entities[j].depth() > entities[j+1].depth()) {
                Entity temp = entities[j];
                entities[j] = entities[j+1];
                entities[j+1] = temp;
            }
        }
    }
}

void renderEntities() {
    // Sort by depth
    sortEntitiesByDepth();

    // Render back-to-front
    for (int i = 0; i < num_entities; i++) {
        if (!entities[i].active) continue;

        int screen_x = (entities[i].world_x - entities[i].world_y) * 16 - camera_x;
        int screen_y = (entities[i].world_x + entities[i].world_y) * 8 - camera_y;

        blitSprite(framebuffer, entities[i].sprite, screen_x, screen_y);
    }
}
```

---

## Common Mistakes

### 1. Not Sorting
```cpp
// ❌ BAD: Render in creation order
for (int i = 0; i < num_entities; i++) {
    renderEntity(&entities[i]);
}
// Result: Visual artifacts, wrong overlap
```

### 2. Wrong Sort Direction
```cpp
// ❌ BAD: Larger depth first (front-to-back)
return a.depth() > b.depth();

// ✅ GOOD: Smaller depth first (back-to-front)
return a.depth() < b.depth();
```

### 3. Forgetting Camera Offset
```cpp
// ❌ BAD: Uses world coordinates directly
blitSprite(framebuffer, sprite, world_x * 16, world_y * 8);

// ✅ GOOD: Applies camera offset
int screen_x = (world_x - world_y) * 16 - camera_x;
int screen_y = (world_x + world_y) * 8 - camera_y;
blitSprite(framebuffer, sprite, screen_x, screen_y);
```

---

## Performance Summary

**Target (from CLAUDE.md):** 10-20 entities on screen

**Sorting cost:**
- 20 entities with bubble sort: ~5µs
- 20 entities with std::sort: ~2µs

**Rendering cost:**
- 20 entities × 3µs (Flash read) = 60µs

**Total:** ~65µs out of 16,667µs frame budget (0.4%)

**Depth sorting is cheap - don't skip it!**

---

## Next Steps

- **[Isometric Rendering](rendering.md)** - Tile rendering and coordinate conversion
- **CLAUDE.md** - See depth sorting section for project-specific details

---

## Reference

**Depth formula:**
```cpp
depth = world_x + world_y
```

**Sort order:** Smaller depth first (back-to-front)

**Render pipeline:**
1. Clear framebuffer
2. Render tiles (back-to-front)
3. Sort entities by depth
4. Render entities (back-to-front)
5. Render UI
6. Send to display
