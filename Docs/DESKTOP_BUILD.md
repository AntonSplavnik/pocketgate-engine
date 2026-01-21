# Desktop Build (GLFW/OpenGL)

Development build for testing engine features on desktop without Pico hardware.

---

## Purpose

The desktop build allows you to:
- Test rendering, movement, and game logic without flashing the Pico
- Iterate faster during development (compile and run instantly)
- Debug with standard tools (printf, debuggers, profilers)
- Validate algorithms before deploying to embedded hardware

The same `framebuffer.cpp` and rendering code runs on both platforms - only the display output differs.

---

## Requirements

**macOS:**
```bash
brew install glfw
```

**Linux:**
```bash
sudo apt install libglfw3-dev libgl1-mesa-dev
```

---

## Build Commands

```bash
# Build only
make -f Makefile.desktop

# Build and run
make -f Makefile.desktop run

# Clean build artifacts
make -f Makefile.desktop clean
```

The `-f` flag tells make to use `Makefile.desktop` instead of the default `Makefile`.

Output binary: `build_desktop/pocket_gate`

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                     Desktop Build                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│   ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│   │ framebuffer  │    │   OpenGL     │    │    GLFW      │  │
│   │   (RGB565)   │ -> │   Texture    │ -> │   Window     │  │
│   │   40 KB      │    │ glTexImage2D │    │  640x512     │  │
│   └──────────────┘    └──────────────┘    └──────────────┘  │
│         ↑                                        ↓          │
│   Engine renders                           User sees        │
│   to back buffer                           4x scaled        │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

**Platform comparison:**

| Component | Pico Hardware | Desktop |
|-----------|---------------|---------|
| Display output | ST7735 via SPI | GLFW window |
| Framebuffer | Same RGB565 (40KB) | Same RGB565 (40KB) |
| Frame transfer | `spi_write_blocking()` | `glTexImage2D()` |
| Input | GPIO buttons | Keyboard (WASD) |
| Resolution | 160x128 native | 640x512 (4x scaled) |

---

## Initialization Sequence

The `main()` function sets up the desktop environment in this order:

### 1. GLFW Initialization
```cpp
glfwSetErrorCallback(error_callback);  // Register error handler
if (!glfwInit()) return -1;            // Initialize GLFW library
```
GLFW handles window creation, input, and the OpenGL context.

### 2. OpenGL Context Setup
```cpp
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
```
Uses OpenGL 2.0 for compatibility. Legacy OpenGL is sufficient for displaying a 2D texture.

### 3. Window Creation
```cpp
g_window = glfwCreateWindow(160*4, 128*4, "PocketGateEngine", NULL, NULL);
glfwMakeContextCurrent(g_window);
```
Creates a 640x512 window (4x the native 160x128 resolution) and makes its OpenGL context current.

### 4. OpenGL Function Loading
```cpp
gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
```
GLAD loads OpenGL function pointers. Required before calling any `gl*` functions.

### 5. Texture Setup
```cpp
glGenTextures(1, &g_texture);
glBindTexture(GL_TEXTURE_2D, g_texture);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
```
Creates a texture to hold the framebuffer data. `GL_NEAREST` filtering gives crisp pixel-art scaling (no blurring).

### 6. Input Callback
```cpp
glfwSetKeyCallback(g_window, key_callback);
```
Registers ESC key to close the window.

---

## Frame Presentation

The `present_frame()` function displays the engine's framebuffer on screen:

```cpp
void present_frame() {
    // 1. Upload framebuffer to GPU texture
    glBindTexture(GL_TEXTURE_2D, g_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160, 128, 0,
                 GL_RGB, GL_UNSIGNED_SHORT_5_6_5, get_front_buffer());

    // 2. Draw fullscreen quad with texture
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
    glEnd();

    // 3. Swap front/back buffers and poll input
    glfwSwapBuffers(g_window);
    glfwPollEvents();
}
```

**Key point:** `GL_UNSIGNED_SHORT_5_6_5` tells OpenGL the data is RGB565 format - the same format the engine uses internally. No conversion needed.

---

## Game Loop Structure

A typical test function follows this pattern:

```cpp
void some_test() {
    // Initial render
    fill_with_color(0x0000);           // Clear back buffer
    draw_sprite_alpha(...);            // Draw to back buffer
    swap_buffers();                    // Swap front/back
    present_frame();                   // Display on screen

    // Main loop
    while (!glfwWindowShouldClose(g_window)) {
        glfwPollEvents();              // Process input events

        // Update game state
        handle_movement(sprite);

        // Render frame
        fill_with_color(0x0000);       // Clear
        draw_sprite_alpha(...);        // Draw
        fps_counter();                 // Print FPS
        swap_buffers();                // Swap buffers
        present_frame();               // Display
    }
}
```

**Important:** Always call `glfwPollEvents()` every frame, even if not rendering. Otherwise the window becomes unresponsive.

---

## Input Handling

Movement is handled via `glfwGetKey()` polling:

```cpp
bool handle_movement(Rectangle& rect) {
    bool moved = false;
    if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS && rect.y > 0) {
        rect.y -= 1;
        moved = true;
    }
    // ... A, S, D keys similarly
    return moved;
}
```

| Key | Action |
|-----|--------|
| W | Move up (Y-) |
| A | Move left (X-) |
| S | Move down (Y+) |
| D | Move right (X+) |
| ESC | Quit |

---

## Vsync Control

```cpp
glfwSwapInterval(1);  // Enable vsync - 60 FPS cap
glfwSwapInterval(0);  // Disable vsync - uncapped FPS
```

**Note:** macOS enables vsync by default even without explicit call. `glfwSwapBuffers()` blocks until the next display refresh.

---

## Available Test Functions

Uncomment in `main()` to run different tests:

| Function | Description |
|----------|-------------|
| `color_test()` | Cycles through solid colors |
| `random_pixels_test()` | Draws 5000 random pixels |
| `line_test()` | Tests line drawing |
| `rectangle_test()` | Tests rectangle drawing |
| `sprite_test()` | Draws skeleton sprites at corners and center |
| `movement_tracking_test_regular()` | Move a rectangle with WASD |
| `movement_tracking_test_sprite_wizard()` | Move wizard sprite with WASD |
| `bresenham_line_drawing_test()` | Tests Bresenham line algorithm |
| `diamond_outline_test()` | Tests isometric diamond outline |

---

## File Structure

```
main_glfw.cpp       # Desktop entry point, tests, game loop
platform_desktop.h  # Screen dimension defines (128x160)
Makefile.desktop    # Build configuration
glad/               # OpenGL function loader
  ├── include/
  │   ├── glad/glad.h
  │   └── KHR/khrplatform.h
  └── src/glad.c
build_desktop/      # Build output (gitignored)
  ├── obj/          # Object files
  └── pocket_gate   # Executable
```

---

## Debugging Tips

**FPS counter:** The `fps_counter()` function prints FPS to terminal every second:
```
FPS: 60
FPS: 61
FPS: 59
```

**Printf debugging:** Use standard `printf()` - output goes to terminal.

**Frame-by-frame:** Add a key check to step one frame at a time:
```cpp
if (glfwGetKey(g_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    // render one frame
}
```

---

## See Also

- **[TECHNICAL_SPECS.md](TECHNICAL_SPECS.md)** - Hardware specifications
- **[hardware/rgb565.md](hardware/rgb565.md)** - Color format details
- **[graphics/alpha_channel.md](graphics/alpha_channel.md)** - Sprite transparency
