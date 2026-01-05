# Hardware Upgrade Plan

## Overview
This document outlines potential hardware upgrades for the TriggEngine console to improve performance, usability, and user experience.

---

## 1. Microcontroller Upgrade

### Current: Raspberry Pi Pico (RP2040)
- Dual-core ARM Cortex-M0+ @ 133MHz
- 264KB SRAM
- 2MB Flash

### Proposed: Raspberry Pi Pico 2 (RP2350) or Better

#### Option A: Raspberry Pi Pico 2 (RP2350)
- Dual-core ARM Cortex-M33 @ 150MHz
- 520KB SRAM (2x current)
- 4MB Flash (2x current)
- Hardware floating-point unit
- Better performance for graphics operations
- Pin-compatible with Pico (easy upgrade)

#### Option B: ESP32-S3
- Dual-core Xtensa LX7 @ 240MHz
- 512KB SRAM
- WiFi/Bluetooth built-in (multiplayer potential)
- Better performance for complex games

**Recommendation**: Start with Pico 2 for compatibility, consider ESP32-S3 for future revisions

---

## 2. Display Upgrade

### Current: ST7735 128x160 SPI
- 1.8" TFT
- 128x160 resolution
- Standard TN panel

### Proposed: Larger IPS Display

#### Option A: ST7789 240x240 IPS
- 1.54" or 1.69" IPS display
- 240x240 resolution (~3.5x more pixels)
- Better viewing angles
- Superior color reproduction
- Same SPI interface (easy integration)

#### Option B: ST7789 240x320 IPS
- 2.0" or 2.4" IPS display
- 240x320 resolution (~4.7x more pixels)
- Wider aspect ratio for gameplay
- May require larger form factor

**Benefits**:
- Wider viewing angles (IPS technology)
- Better color accuracy
- More screen real estate for detailed isometric graphics
- Higher resolution allows for more detailed sprites

**Requirements**:
- Update SCREEN_WIDTH/SCREEN_HEIGHT constants
- Larger framebuffer (240×240 = 115KB vs current 40KB)
- More Flash memory for higher-res assets
- Pico 2's extra RAM makes this feasible

### 2.1 Understanding Display Resolution & PPI

#### What is PPI?
**PPI (Pixels Per Inch)** is the relationship between resolution and physical display size:
```
PPI = diagonal_in_pixels / diagonal_in_inches
```

- **Higher PPI** = More pixels packed per inch = Smaller individual pixels
- **Lower PPI** = Fewer pixels per inch = Larger individual pixels
- **PPI varies between display models** - it's NOT standardized

#### Current Display Analysis
**ST7735 128×160 (1.8" diagonal)**:
- Resolution: 128×160 pixels
- PPI ≈ 114
- Framebuffer: 40KB (128×160×2 bytes)
- Physical pixel size: ~0.22mm

#### Common Upgrade Options with PPI Analysis

| Display | Size | Resolution | PPI | Framebuffer | Physical Pixel Size |
|---------|------|------------|-----|-------------|-------------------|
| ST7735 (current) | 1.8" | 128×160 | ~114 | 40KB | ~0.22mm |
| ST7789 | 2.4" | 240×240 | ~141 | 115KB | ~0.18mm |
| ST7789 | 1.54" | 240×240 | ~220 | 115KB | ~0.12mm |
| ILI9341 | 2.4" | 240×320 | ~167 | 154KB | ~0.15mm |
| ILI9341 | 2.8" | 240×320 | ~143 | 154KB | ~0.18mm |
| ST7789 | 4.0" | 320×240 | ~100 | 154KB | ~0.25mm |
| Higher-res | 4.0" | 480×320 | ~144 | 307KB | ~0.18mm |

#### How Display Upgrades Affect Sprites

When upgrading displays, **two independent factors** affect how sprites appear:

**Factor 1: Resolution Change (View Area)**
- Higher resolution = More pixels = Larger view area
- A 16×16 pixel sprite remains 16×16 pixels
- You see **more of the game world** (camera shows larger area)

**Factor 2: Physical Size Change (Sprite Physical Size)**
- Larger physical display = Bigger pixels (usually)
- Same 16×16 sprite appears **physically larger or smaller** depending on PPI
- Lower PPI = sprite is physically bigger and easier to see
- Higher PPI = sprite is physically smaller but sharper

#### Practical Examples

**Example 1: Upgrade to 4" Display (320×240)**
- Resolution increases: 128×160 → 320×240 (2.5× width, 1.5× height)
- View area: See ~2.5× more horizontally, 1.5× more vertically
- PPI decreases: ~114 → ~100 (pixels get bigger)
- **Result**: More view area + sprites are physically comfortable to see

**Example 2: Upgrade to 2.4" Display (240×320)**
- Resolution increases: 128×160 → 240×320 (1.9× both dimensions)
- View area: See ~1.9× more of the game world
- PPI increases: ~114 → ~167 (pixels get smaller)
- **Result**: More view area but sprites are physically smaller (sharper, more detailed)

#### Choosing the Right Display

**For comfortable gameplay without scaling sprites:**
- Target PPI: 80-130 (similar to current display)
- Recommended: 2.4"-4.0" displays with 240×240 or 320×240 resolution
- These provide more view area while keeping sprites physically visible

**For maximum detail (may need larger sprites):**
- Higher PPI displays (150+) provide sharper graphics
- May require upscaling sprites or creating higher-resolution assets
- Better for displays with more RAM budget

#### Memory Constraints

**Framebuffer Memory = Width × Height × 2 bytes (RGB565)**

On Raspberry Pi Pico (264KB RAM):
- 128×160: 40KB (double buffered: 80KB) ✓ Current
- 240×240: 115KB (double buffered: 230KB) ⚠️ Tight on Pico 1
- 320×240: 154KB (double buffered: 308KB) ✗ Exceeds Pico 1 RAM

On Raspberry Pi Pico 2 (520KB RAM):
- 240×320: 154KB (double buffered: 308KB) ✓ Comfortable
- 480×320: 307KB (double buffered: 614KB) ✗ Too large even for Pico 2

**Practical Limit**: 320×240 is near maximum for double buffering on Pico 2

---

## 3. Input System Upgrades

### Current Configuration
- Individual tactile buttons
- PSP-like horizontal layout
- No hardware debouncing

### Proposed Changes

#### 3.1 D-Pad (Cross/Directional Pad)
**Replace left-side discrete buttons with proper D-pad**
- Better tactile feedback for directional input
- More ergonomic for isometric movement
- Standard gaming control scheme
- Options:
  - Single membrane D-pad
  - Four mechanical switches in cross formation
  - Alps SKQE series low-profile tactile switches

#### 3.2 Hardware Debouncing
**Add capacitors to all button inputs**
- 100nF ceramic capacitor per button (between GPIO and GND)
- Optional: RC network (10kΩ + 100nF) for cleaner signals
- Reduces need for software debouncing
- Improves input reliability
- Faster input response

#### 3.3 Additional Buttons
**Add standard console buttons**:
- **START button**: Pause/menu access
- **SELECT button**: Secondary menu/options
- **L/R shoulder buttons**: Optional, for additional actions

#### 3.4 Button Layout
```
Current (PSP-style):
[D-Pad Area]  [SCREEN]  [Action Buttons]
    ↕↔                      A B
                            X Y

Proposed (Game Boy Color style - vertical):
     [SCREEN]

     [D-Pad]  [A]
              [B]

  [SELECT] [START]
```

---

## 4. Form Factor & Orientation

### Current: PSP-style (Horizontal/Landscape)
- Wide horizontal layout
- Side-by-side controls

### Proposed: Game Boy Color Style (Vertical/Portrait)

#### Option A: Portrait Orientation (Recommended)
**Advantages**:
- More compact and portable
- Better one-handed grip potential
- Familiar retro gaming form factor
- Natural fit for 240x320 display (vertical)
- Easier to pocket

**Considerations**:
- May need to adjust isometric camera for portrait aspect
- UI elements need portrait-friendly layout

#### Option B: Keep Landscape
**Advantages**:
- Wider field of view for isometric games
- More traditional console feel
- No code changes for aspect ratio

**Decision Point**: Test both with cardboard prototype before finalizing

---

## 5. Audio Upgrade

### Current: Single Speaker
- Mono audio output

### Proposed: Enhanced Audio System

#### Option A: Stereo Speakers
- Two small speakers (left/right)
- Stereo sound effects and music
- More immersive experience
- Requires second PWM channel

#### Option B: Better Single Speaker + 3.5mm Jack
- Larger/better quality speaker
- Headphone jack for private gaming
- Simpler implementation

**Recommendation**: Start with Option B (headphone jack), add stereo in future revision

---

## 6. Power System Upgrade

### Current: AAA Batteries
- 2-3x AAA batteries
- Frequent replacement needed
- Environmental waste
- Variable voltage as batteries drain

### Proposed: Rechargeable Lithium Battery

#### Recommended Solution: Li-Po/Li-Ion Battery
**Battery Options**:
- 18650 Li-Ion cell (3.7V, 2000-3500mAh)
- Li-Po pouch cell (3.7V, 1000-2000mAh)
- Custom capacity based on form factor

**Required Components**:
- **Battery Management System (BMS)**:
  - TP4056 charging module (simple, cheap)
  - MCP73831 single-cell charger IC
  - Overcharge/overdischarge protection

- **Power Regulation**:
  - 3.3V buck converter (TPS63000 or similar)
  - Efficient switching regulator for battery life

- **Charging**:
  - USB-C charging port
  - LED charging indicator (red=charging, green=full)
  - Charge current: 500mA-1A

- **Battery Monitoring**:
  - Voltage divider to ADC for battery level
  - Low battery warning system
  - Fuel gauge IC (optional): MAX17048 for accurate %

**Benefits**:
- Longer play time (3-6 hours depending on capacity)
- Rechargeable via USB
- Cost savings over time
- Consistent voltage/performance
- Battery level indicator possible

**Safety Considerations**:
- Proper BMS with protection circuits
- Quality battery cells only
- Temperature monitoring
- Proper enclosure design

---

## 7. Additional Potential Upgrades

### 7.1 Haptic Feedback
- Small vibration motor
- Tactile feedback for game events
- Low power consumption (50-100mA peak)

### 7.2 MicroSD Card Slot
- Expandable storage for game saves
- Load custom games/levels
- Asset streaming for large games
- FAT32 filesystem support

### 7.3 RGB Status LED
- WS2812B or similar
- Show battery level, game state, notifications
- Low cost, high visibility

### 7.4 External GPIO Header
- Expansion port for accessories
- I2C/SPI/UART breakout
- Debugging interface
- Future peripheral support

---

## 8. Implementation Priority

### Phase 1: Essential Upgrades
1. **Microcontroller**: Upgrade to Pico 2
2. **Power System**: Li-Po battery + USB-C charging
3. **Display**: 240x240 ST7789 IPS
4. **Input**: Hardware debouncing capacitors

### Phase 2: Ergonomics & Usability
5. **D-Pad**: Replace discrete buttons
6. **Start/Select buttons**: Add menu controls
7. **Form Factor**: Decide orientation (prototype both)

### Phase 3: Enhanced Features
8. **Audio**: Headphone jack + better speaker
9. **Battery Monitor**: Add fuel gauge/indicator
10. **Status LED**: RGB notification LED

### Phase 4: Future Enhancements
11. **Haptic feedback**: Vibration motor
12. **MicroSD**: Expandable storage
13. **Stereo audio**: Dual speakers
14. **GPIO expansion**: Developer port

---

## 9. Estimated Costs (USD)

| Component | Current | Upgraded | Difference |
|-----------|---------|----------|------------|
| MCU | Pico ($4) | Pico 2 ($5) | +$1 |
| Display | ST7735 ($4) | ST7789 IPS ($8) | +$4 |
| Battery System | AAA holders ($1) | Li-Po + BMS + USB-C ($8) | +$7 |
| Input | Buttons ($2) | D-pad + caps + buttons ($5) | +$3 |
| Audio | Speaker ($1) | Speaker + jack ($3) | +$2 |
| Misc | - | Status LED, voltage reg ($2) | +$2 |
| **Total** | **~$12** | **~$31** | **+$19** |

*Note: Prices are estimates for hobbyist quantities. Bulk pricing would reduce costs.*

---

## 10. Software Implications

### Engine Changes Required
- [x] Display abstraction already supports resolution changes
- [ ] Update SCREEN_WIDTH/SCREEN_HEIGHT constants
- [ ] Scale or redesign assets for higher resolution
- [ ] Adjust framebuffer allocation (use Pico 2's extra RAM)
- [ ] Add battery monitoring system
- [ ] Implement charging status display
- [ ] Add START/SELECT button handling
- [ ] D-pad input mapping (if different from discrete buttons)
- [ ] Low battery warning system

### Asset Pipeline
- [ ] Generate 240x240 optimized sprites
- [ ] Consider asset compression for larger display
- [ ] Use Flash more efficiently with higher-res graphics

---

## 11. Testing Plan

### Hardware Prototyping
1. **Breadboard Phase**: Test each component individually
   - Pico 2 with new display
   - Battery charging circuit
   - Button debouncing effectiveness

2. **Form Factor Mockup**: Cardboard/3D printed prototype
   - Test portrait vs landscape
   - Ergonomics testing
   - Button placement validation

3. **PCB Design**: Custom board layout
   - Integrate all components
   - Optimize routing for signal integrity
   - Power distribution design

### Software Testing
1. Test display driver with ST7789
2. Benchmark frame rates with larger resolution
3. Verify battery monitoring accuracy
4. Input latency testing with debouncing

---

## Notes
- Maintain backward compatibility where possible
- Design modular: allow incremental upgrades
- Document all pin assignments
- Keep schematics updated
- Consider manufacturing if design proves popular

---

**Last Updated**: 2026-01-01
**Status**: Proposal/Planning Phase
