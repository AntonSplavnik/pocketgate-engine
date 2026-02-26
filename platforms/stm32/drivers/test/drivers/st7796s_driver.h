/**
 * @file st7796s_driver.h
 * @brief ST7796S 4" TFT LCD Driver for STM32H743
 *
 * 480x320 IPS display with SPI interface
 */

#ifndef ST7796S_DRIVER_H
#define ST7796S_DRIVER_H

#include "stm32h7xx_hal.h"
#include "board_config.h"
#include <cstdint>

namespace ST7796S {

// =============================================================================
// Display Constants
// =============================================================================

constexpr uint16_t WIDTH = DISPLAY_WIDTH;
constexpr uint16_t HEIGHT = DISPLAY_HEIGHT;
constexpr uint32_t PIXEL_COUNT = WIDTH * HEIGHT;
constexpr uint32_t BUFFER_SIZE = PIXEL_COUNT * 2;  // RGB565

// =============================================================================
// ST7796S Commands
// =============================================================================

enum Command : uint8_t {
    NOP         = 0x00,
    SWRESET     = 0x01,  // Software Reset
    RDDID       = 0x04,  // Read Display ID
    RDDST       = 0x09,  // Read Display Status
    SLPIN       = 0x10,  // Sleep In
    SLPOUT      = 0x11,  // Sleep Out
    PTLON       = 0x12,  // Partial Mode On
    NORON       = 0x13,  // Normal Mode On
    INVOFF      = 0x20,  // Inversion Off
    INVON       = 0x21,  // Inversion On
    DISPOFF     = 0x28,  // Display Off
    DISPON      = 0x29,  // Display On
    CASET       = 0x2A,  // Column Address Set
    RASET       = 0x2B,  // Row Address Set
    RAMWR       = 0x2C,  // Memory Write
    RAMRD       = 0x2E,  // Memory Read
    PTLAR       = 0x30,  // Partial Area
    VSCRDEF     = 0x33,  // Vertical Scroll Definition
    TEOFF       = 0x34,  // Tearing Effect Off
    TEON        = 0x35,  // Tearing Effect On
    MADCTL      = 0x36,  // Memory Access Control
    VSCRSADD    = 0x37,  // Vertical Scroll Start Address
    IDMOFF      = 0x38,  // Idle Mode Off
    IDMON       = 0x39,  // Idle Mode On
    COLMOD      = 0x3A,  // Pixel Format Set
    RAMWRC      = 0x3C,  // Memory Write Continue
    RAMRDC      = 0x3E,  // Memory Read Continue
    TESCAN      = 0x44,  // Set Tear Scanline
    RDTESCAN    = 0x45,  // Read Tear Scanline
    WRDISBV     = 0x51,  // Write Display Brightness
    RDDISBV     = 0x52,  // Read Display Brightness
    WRCTRLD     = 0x53,  // Write CTRL Display
    RDCTRLD     = 0x54,  // Read CTRL Display
    WRCABC      = 0x55,  // Write CABC
    RDCABC      = 0x56,  // Read CABC
    WRCABCMB    = 0x5E,  // Write CABC Min Brightness
    RDCABCMB    = 0x5F,  // Read CABC Min Brightness
    RDID1       = 0xDA,  // Read ID1
    RDID2       = 0xDB,  // Read ID2
    RDID3       = 0xDC,  // Read ID3

    // Extended commands
    IFMODE      = 0xB0,  // Interface Mode Control
    FRMCTR1     = 0xB1,  // Frame Rate Control 1
    FRMCTR2     = 0xB2,  // Frame Rate Control 2
    FRMCTR3     = 0xB3,  // Frame Rate Control 3
    INVCTR      = 0xB4,  // Display Inversion Control
    BPC         = 0xB5,  // Blanking Porch Control
    DFC         = 0xB6,  // Display Function Control
    EM          = 0xB7,  // Entry Mode Set
    PWR1        = 0xC0,  // Power Control 1
    PWR2        = 0xC1,  // Power Control 2
    PWR3        = 0xC2,  // Power Control 3
    VCMPCTL     = 0xC5,  // VCOM Control
    VCMOFFSET   = 0xC6,  // VCOM Offset
    NVMADW      = 0xD0,  // NVM Address/Data Write
    NVMBPROG    = 0xD1,  // NVM Byte Program
    NVMSTRD     = 0xD2,  // NVM Status Read
    RDID4       = 0xD3,  // Read ID4
    PGC         = 0xE0,  // Positive Gamma Control
    NGC         = 0xE1,  // Negative Gamma Control
    DGC1        = 0xE2,  // Digital Gamma Control 1
    DGC2        = 0xE3,  // Digital Gamma Control 2
    DOCA        = 0xE8,  // Display Output Ctrl Adjust
    CSCON       = 0xF0,  // Command Set Control
};

// MADCTL bits
enum MADCTL_Bits : uint8_t {
    MADCTL_MY   = 0x80,  // Row Address Order
    MADCTL_MX   = 0x40,  // Column Address Order
    MADCTL_MV   = 0x20,  // Row/Column Exchange
    MADCTL_ML   = 0x10,  // Vertical Refresh Order
    MADCTL_BGR  = 0x08,  // BGR Order (vs RGB)
    MADCTL_MH   = 0x04,  // Horizontal Refresh Order
};

// Color format
enum ColorMode : uint8_t {
    COLOR_12BIT = 0x03,  // 12-bit/pixel
    COLOR_16BIT = 0x05,  // 16-bit/pixel (RGB565)
    COLOR_18BIT = 0x06,  // 18-bit/pixel
};

// =============================================================================
// Rotation Modes
// =============================================================================

enum Rotation : uint8_t {
    ROTATION_0   = 0,    // Portrait
    ROTATION_90  = 1,    // Landscape
    ROTATION_180 = 2,    // Portrait inverted
    ROTATION_270 = 3,    // Landscape inverted
};

// =============================================================================
// Color Definitions (RGB565)
// =============================================================================

constexpr uint16_t COLOR_BLACK   = 0x0000;
constexpr uint16_t COLOR_WHITE   = 0xFFFF;
constexpr uint16_t COLOR_RED     = 0xF800;
constexpr uint16_t COLOR_GREEN   = 0x07E0;
constexpr uint16_t COLOR_BLUE    = 0x001F;
constexpr uint16_t COLOR_YELLOW  = 0xFFE0;
constexpr uint16_t COLOR_CYAN    = 0x07FF;
constexpr uint16_t COLOR_MAGENTA = 0xF81F;
constexpr uint16_t COLOR_GRAY    = 0x8410;

/**
 * @brief Convert RGB888 to RGB565
 */
constexpr uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// =============================================================================
// Driver Class
// =============================================================================

class Driver {
public:
    /**
     * @brief Initialize the display driver
     * @param hspi Pointer to SPI handle
     * @return true if initialization successful
     */
    bool init(SPI_HandleTypeDef* hspi);

    /**
     * @brief Set display rotation
     * @param rotation Rotation mode (0, 90, 180, 270)
     */
    void setRotation(Rotation rotation);

    /**
     * @brief Turn display on
     */
    void displayOn();

    /**
     * @brief Turn display off
     */
    void displayOff();

    /**
     * @brief Fill entire screen with color
     * @param color RGB565 color value
     */
    void fillScreen(uint16_t color);

    /**
     * @brief Set address window for drawing
     * @param x0 Start X
     * @param y0 Start Y
     * @param x1 End X
     * @param y1 End Y
     */
    void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

    /**
     * @brief Write pixel data to display (after setAddressWindow)
     * @param data Pointer to RGB565 pixel data
     * @param len Number of pixels
     */
    void writePixels(const uint16_t* data, uint32_t len);

    /**
     * @brief Write single pixel
     * @param x X coordinate
     * @param y Y coordinate
     * @param color RGB565 color
     */
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Fill rectangle with color
     * @param x Start X
     * @param y Start Y
     * @param w Width
     * @param h Height
     * @param color RGB565 color
     */
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

    /**
     * @brief Send framebuffer to display
     * @param buffer Pointer to framebuffer (RGB565, WxH pixels)
     */
    void sendFramebuffer(const uint16_t* buffer);

    /**
     * @brief Send framebuffer using DMA (non-blocking)
     * @param buffer Pointer to framebuffer
     * @return true if DMA transfer started
     */
    bool sendFramebufferDMA(const uint16_t* buffer);

    /**
     * @brief Check if DMA transfer is complete
     * @return true if transfer complete or no transfer in progress
     */
    bool isDMAComplete();

    /**
     * @brief Get current width (depends on rotation)
     */
    uint16_t getWidth() const { return m_width; }

    /**
     * @brief Get current height (depends on rotation)
     */
    uint16_t getHeight() const { return m_height; }

private:
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    void writeDataBuffer(const uint8_t* data, uint32_t len);

    void hardwareReset();
    void softwareReset();
    void initSequence();

    SPI_HandleTypeDef* m_hspi = nullptr;
    uint16_t m_width = WIDTH;
    uint16_t m_height = HEIGHT;
    Rotation m_rotation = ROTATION_90;  // Default landscape
    volatile bool m_dmaInProgress = false;
};

// =============================================================================
// Global Instance
// =============================================================================

extern Driver display;

}  // namespace ST7796S

#endif // ST7796S_DRIVER_H
