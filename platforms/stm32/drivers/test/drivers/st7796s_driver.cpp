/**
 * @file st7796s_driver.cpp
 * @brief ST7796S Display Driver Implementation
 */

#include "st7796s_driver.h"

namespace ST7796S {

// Global instance
Driver display;

// =============================================================================
// Public Methods
// =============================================================================

bool Driver::init(SPI_HandleTypeDef* hspi)
{
    if (hspi == nullptr) {
        return false;
    }

    m_hspi = hspi;

    // Hardware reset
    hardwareReset();

    // Initialize display
    initSequence();

    // Set default rotation (landscape for gaming)
    setRotation(ROTATION_90);

    // Turn on display
    displayOn();

    return true;
}

void Driver::setRotation(Rotation rotation)
{
    m_rotation = rotation;

    uint8_t madctl = MADCTL_BGR;  // BGR color order for ST7796S

    switch (rotation) {
        case ROTATION_0:    // Portrait
            madctl |= MADCTL_MX;
            m_width = WIDTH;
            m_height = HEIGHT;
            break;

        case ROTATION_90:   // Landscape (default for gaming)
            madctl |= MADCTL_MV;
            m_width = HEIGHT;
            m_height = WIDTH;
            break;

        case ROTATION_180:  // Portrait inverted
            madctl |= MADCTL_MY;
            m_width = WIDTH;
            m_height = HEIGHT;
            break;

        case ROTATION_270:  // Landscape inverted
            madctl |= MADCTL_MV | MADCTL_MX | MADCTL_MY;
            m_width = HEIGHT;
            m_height = WIDTH;
            break;
    }

    writeCommand(Command::MADCTL);
    writeData(madctl);
}

void Driver::displayOn()
{
    writeCommand(Command::DISPON);
    HAL_Delay(20);
}

void Driver::displayOff()
{
    writeCommand(Command::DISPOFF);
}

void Driver::fillScreen(uint16_t color)
{
    fillRect(0, 0, m_width, m_height, color);
}

void Driver::setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // Column address set
    writeCommand(Command::CASET);
    writeData16(x0);
    writeData16(x1);

    // Row address set
    writeCommand(Command::RASET);
    writeData16(y0);
    writeData16(y1);

    // Write to RAM
    writeCommand(Command::RAMWR);
}

void Driver::writePixels(const uint16_t* data, uint32_t len)
{
    LCD_DC_DATA();
    LCD_CS_LOW();

    // Send pixel data (convert to big-endian)
    for (uint32_t i = 0; i < len; i++) {
        uint16_t pixel = data[i];
        uint8_t buf[2] = {
            static_cast<uint8_t>(pixel >> 8),
            static_cast<uint8_t>(pixel & 0xFF)
        };
        HAL_SPI_Transmit(m_hspi, buf, 2, HAL_MAX_DELAY);
    }

    LCD_CS_HIGH();
}

void Driver::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= m_width || y >= m_height) return;

    setAddressWindow(x, y, x, y);

    LCD_DC_DATA();
    LCD_CS_LOW();

    uint8_t buf[2] = {
        static_cast<uint8_t>(color >> 8),
        static_cast<uint8_t>(color & 0xFF)
    };
    HAL_SPI_Transmit(m_hspi, buf, 2, HAL_MAX_DELAY);

    LCD_CS_HIGH();
}

void Driver::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= m_width || y >= m_height) return;
    if (x + w > m_width) w = m_width - x;
    if (y + h > m_height) h = m_height - y;

    setAddressWindow(x, y, x + w - 1, y + h - 1);

    LCD_DC_DATA();
    LCD_CS_LOW();

    uint8_t buf[2] = {
        static_cast<uint8_t>(color >> 8),
        static_cast<uint8_t>(color & 0xFF)
    };

    uint32_t pixels = static_cast<uint32_t>(w) * h;

    // For large fills, use a buffer approach
    if (pixels > 32) {
        // Fill a line buffer
        uint8_t lineBuf[64];  // 32 pixels
        for (int i = 0; i < 32; i++) {
            lineBuf[i * 2] = buf[0];
            lineBuf[i * 2 + 1] = buf[1];
        }

        while (pixels >= 32) {
            HAL_SPI_Transmit(m_hspi, lineBuf, 64, HAL_MAX_DELAY);
            pixels -= 32;
        }
    }

    // Remaining pixels
    while (pixels--) {
        HAL_SPI_Transmit(m_hspi, buf, 2, HAL_MAX_DELAY);
    }

    LCD_CS_HIGH();
}

void Driver::sendFramebuffer(const uint16_t* buffer)
{
    setAddressWindow(0, 0, m_width - 1, m_height - 1);

    LCD_DC_DATA();
    LCD_CS_LOW();

    // Send entire framebuffer
    // Note: Need to swap bytes for big-endian SPI
    uint32_t pixels = static_cast<uint32_t>(m_width) * m_height;

    // Use a temp buffer for byte swapping
    uint8_t swapBuf[512];
    const uint8_t* src = reinterpret_cast<const uint8_t*>(buffer);

    while (pixels > 0) {
        uint32_t chunk = (pixels > 256) ? 256 : pixels;

        // Swap bytes
        for (uint32_t i = 0; i < chunk; i++) {
            swapBuf[i * 2] = src[i * 2 + 1];      // High byte
            swapBuf[i * 2 + 1] = src[i * 2];      // Low byte
        }

        HAL_SPI_Transmit(m_hspi, swapBuf, chunk * 2, HAL_MAX_DELAY);

        src += chunk * 2;
        pixels -= chunk;
    }

    LCD_CS_HIGH();
}

bool Driver::sendFramebufferDMA(const uint16_t* buffer)
{
    if (m_dmaInProgress) {
        return false;
    }

    setAddressWindow(0, 0, m_width - 1, m_height - 1);

    LCD_DC_DATA();
    LCD_CS_LOW();

    // Note: For DMA, the framebuffer should already be in big-endian format
    // or use DMA with byte swapping (if supported)
    m_dmaInProgress = true;

    if (HAL_SPI_Transmit_DMA(m_hspi, (uint8_t*)buffer, BUFFER_SIZE) != HAL_OK) {
        m_dmaInProgress = false;
        LCD_CS_HIGH();
        return false;
    }

    return true;
}

bool Driver::isDMAComplete()
{
    if (!m_dmaInProgress) {
        return true;
    }

    if (HAL_SPI_GetState(m_hspi) == HAL_SPI_STATE_READY) {
        m_dmaInProgress = false;
        LCD_CS_HIGH();
        return true;
    }

    return false;
}

// =============================================================================
// Private Methods
// =============================================================================

void Driver::writeCommand(uint8_t cmd)
{
    LCD_DC_CMD();
    LCD_CS_LOW();
    HAL_SPI_Transmit(m_hspi, &cmd, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void Driver::writeData(uint8_t data)
{
    LCD_DC_DATA();
    LCD_CS_LOW();
    HAL_SPI_Transmit(m_hspi, &data, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void Driver::writeData16(uint16_t data)
{
    uint8_t buf[2] = {
        static_cast<uint8_t>(data >> 8),
        static_cast<uint8_t>(data & 0xFF)
    };

    LCD_DC_DATA();
    LCD_CS_LOW();
    HAL_SPI_Transmit(m_hspi, buf, 2, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void Driver::writeDataBuffer(const uint8_t* data, uint32_t len)
{
    LCD_DC_DATA();
    LCD_CS_LOW();
    HAL_SPI_Transmit(m_hspi, const_cast<uint8_t*>(data), len, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void Driver::hardwareReset()
{
    LCD_RST_HIGH();
    HAL_Delay(10);
    LCD_RST_LOW();
    HAL_Delay(10);
    LCD_RST_HIGH();
    HAL_Delay(120);
}

void Driver::softwareReset()
{
    writeCommand(Command::SWRESET);
    HAL_Delay(120);
}

void Driver::initSequence()
{
    // Sleep out
    writeCommand(Command::SLPOUT);
    HAL_Delay(120);

    // Command Set Control - Enable extension command 2 partI
    writeCommand(Command::CSCON);
    writeData(0xC3);

    // Command Set Control - Enable extension command 2 partII
    writeCommand(Command::CSCON);
    writeData(0x96);

    // Memory Access Control
    writeCommand(Command::MADCTL);
    writeData(MADCTL_BGR);

    // Pixel Format - 16 bit RGB565
    writeCommand(Command::COLMOD);
    writeData(0x55);  // 16-bit color

    // Interface Mode Control
    writeCommand(Command::IFMODE);
    writeData(0x80);

    // Display Function Control
    writeCommand(Command::DFC);
    writeData(0x00);
    writeData(0x02);

    // Display Output Ctrl Adjust
    writeCommand(Command::DOCA);
    writeData(0x40);
    writeData(0x8A);
    writeData(0x00);
    writeData(0x00);
    writeData(0x29);
    writeData(0x19);
    writeData(0xA5);
    writeData(0x33);

    // Power Control 2
    writeCommand(Command::PWR2);
    writeData(0x06);

    // Power Control 3
    writeCommand(Command::PWR3);
    writeData(0xA7);

    // VCOM Control
    writeCommand(Command::VCMPCTL);
    writeData(0x18);

    HAL_Delay(120);

    // Positive Gamma Control
    writeCommand(Command::PGC);
    const uint8_t pgc[] = {
        0xF0, 0x09, 0x0B, 0x06, 0x04, 0x15, 0x2F,
        0x54, 0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B
    };
    for (uint8_t b : pgc) writeData(b);

    // Negative Gamma Control
    writeCommand(Command::NGC);
    const uint8_t ngc[] = {
        0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B,
        0x43, 0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B
    };
    for (uint8_t b : ngc) writeData(b);

    HAL_Delay(120);

    // Command Set Control - Disable extension command 2 partI
    writeCommand(Command::CSCON);
    writeData(0x3C);

    // Command Set Control - Disable extension command 2 partII
    writeCommand(Command::CSCON);
    writeData(0x69);

    HAL_Delay(120);

    // Tearing Effect Line ON (for vsync if needed)
    writeCommand(Command::TEON);
    writeData(0x00);

    // Normal Display Mode On
    writeCommand(Command::NORON);
    HAL_Delay(10);

    // Display ON
    writeCommand(Command::DISPON);
    HAL_Delay(120);
}

}  // namespace ST7796S
