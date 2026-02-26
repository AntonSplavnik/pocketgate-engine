  Single Buffer Approach

  Without double buffering:
  ┌─────────┬─────────┬─────────┬─────────┐
  │ Render  │  Send   │ Render  │  Send   │  (serial, not parallel)
  └─────────┴─────────┴─────────┴─────────┘
     ~8ms      ~5ms      ~8ms      ~5ms

  Frame time: ~13ms = ~76 FPS max (still fine for testing)

  Memory Options for Single 40KB Buffer
  ┌──────────┬───────┬────────┬─────────────────────────────────┐
  │  Region  │ Size  │  DMA   │         Recommendation          │
  ├──────────┼───────┼────────┼─────────────────────────────────┤
  │ AXI SRAM │ 512KB │ MDMA   │ Best - plenty of room           │
  ├──────────┼───────┼────────┼─────────────────────────────────┤
  │ SRAM1    │ 128KB │ DMA1/2 │ Good - if using SPI with DMA1/2 │
  ├──────────┼───────┼────────┼─────────────────────────────────┤
  │ DTCM     │ 128KB │ No     │ Only if no DMA (CPU polling)    │
  └──────────┴───────┴────────┴─────────────────────────────────┘
  Quick Single-Buffer Setup

  // Option 1: AXI SRAM (use MDMA for SPI)
  __attribute__((section(".axisram")))
  uint16_t framebuffer[128 * 160];  // 40KB

  // Option 2: SRAM1 (use DMA1/DMA2 for SPI)
  __attribute__((section(".sram1")))
  uint16_t framebuffer[128 * 160];  // 40KB

  // Simple blocking loop (no DMA, just for testing)
  while (1) {
      clear_buffer(framebuffer);
      draw_something(framebuffer);
      send_to_display_blocking(framebuffer);  // CPU waits
  }

  What do you want to work on next?
  1. SPI setup for display communication
  2. Linker script to define memory sections
  3. Display driver (ST7735/ST7789 init sequence)
  4. Something else?   
