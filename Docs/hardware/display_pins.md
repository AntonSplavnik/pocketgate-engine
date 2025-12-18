# Display Pin Descriptions

## Pin Configuration

The ST7735 display connects to the Pico using the following pins:

```cpp
#define PIN_SCK     2      // SPI Clock
#define PIN_MOSI    3      // SPI Data Out
#define PIN_MISO    19     // SPI Data In (SD card only)
#define PIN_BL      18     // Backlight
#define PIN_TFT_CS  20     // Display Chip Select
#define PIN_CARD_CS 21     // SD Card Chip Select
#define PIN_DC      22     // Data/Command
#define PIN_RESET   26     // Display Reset
```

---

## Pin Details

### MOSI (GPIO 3) - Master Out Slave In
- **Purpose:** Pico sends DATA to display
- **Data sent:** Pixel colors, commands, parameters
- **Direction:** Pico → Display (one-way)
- **Example:** Sending the command "fill screen red"

### SCK (GPIO 2) - Serial Clock
- **Purpose:** Timing signal (like a metronome)
- **How it works:** Data changes on each clock pulse
- **Speed:** 62.5 MHz on Trigg console
- **Analogy:** Like a conductor's baton keeping musicians in sync

### CS (GPIO 20) - Chip Select
- **Purpose:** Tells the display "I'm talking to YOU now"
- **LOW (0):** Display listens and processes data
- **HIGH (1):** Display ignores the SPI bus
- **Why needed:** Multiple devices can share the same SPI bus

### DC (GPIO 22) - Data/Command
- **Purpose:** Tells display what type of information you're sending
- **LOW (0):** Command mode (instructions like "clear screen", "set brightness")
- **HIGH (1):** Data mode (actual pixel colors, coordinates)
- **Critical:** Display needs to know if you're giving orders or sending pixels

### RST (GPIO 26) - Reset
- **Purpose:** Restarts the display chip (like rebooting a computer)
- **How to use:** Pulse LOW then HIGH
- **When to use:** At startup, or if display gets stuck
- **Timing:** Hold LOW for 10ms, then HIGH and wait 100ms

### BL (GPIO 18) - Backlight
- **Purpose:** Controls screen brightness
- **HIGH (1):** Backlight ON (screen visible)
- **LOW (0):** Backlight OFF (screen dark, saves power)
- **Note:** Display still works when backlight is off, you just can't see it

### MISO (GPIO 19) - Master In Slave Out
- **Purpose:** Receive data FROM display (not used for ST7735 drawing)
- **Used for:** SD card communication on Trigg console
- **Display drawing:** You only need MOSI (one-way communication)

---

## Display Initialization Sequence

The ST7735 display needs to be told how to operate:

1. **Wake from sleep** - Display starts in low-power mode
2. **Set pixel format** - Tell it we're using RGB565 (16-bit color)
3. **Configure orientation** - Portrait vs landscape, mirroring
4. **Define frame rate** - How fast to refresh (60Hz typical)
5. **Turn on display** - Enable the actual screen output

Without these commands, the display won't show anything even if backlight is on.
