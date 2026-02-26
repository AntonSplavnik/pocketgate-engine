/**
 * @file board_config.h
 * @brief Hardware configuration for WeAct STM32H743VIT6 + 4" ST7796S Display
 *
 * Pin assignments based on: STM32H743_4INCH_DISPLAY_WIRING.md
 */

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Display Configuration
// =============================================================================

#ifndef DISPLAY_WIDTH
#define DISPLAY_WIDTH   480
#endif

#ifndef DISPLAY_HEIGHT
#define DISPLAY_HEIGHT  320
#endif

#define DISPLAY_PIXELS      (DISPLAY_WIDTH * DISPLAY_HEIGHT)
#define FRAMEBUFFER_SIZE    (DISPLAY_PIXELS * 2)  // RGB565 = 2 bytes/pixel

// =============================================================================
// SPI3 - Display & SD Card
// =============================================================================

#define LCD_SPI                     SPI3
#define LCD_SPI_INSTANCE            SPI3

// SPI3 Pins (directly on peripheral - no GPIO config needed for AF)
#define SPI3_SCK_PIN                GPIO_PIN_3
#define SPI3_SCK_PORT               GPIOB
#define SPI3_SCK_AF                 GPIO_AF6_SPI3

#define SPI3_MISO_PIN               GPIO_PIN_4
#define SPI3_MISO_PORT              GPIOB
#define SPI3_MISO_AF                GPIO_AF6_SPI3

#define SPI3_MOSI_PIN               GPIO_PIN_5
#define SPI3_MOSI_PORT              GPIOB
#define SPI3_MOSI_AF                GPIO_AF6_SPI3

// =============================================================================
// I2C1 - Touch Controller
// =============================================================================

#define CTP_I2C                     I2C1
#define CTP_I2C_INSTANCE            I2C1

#define I2C1_SCL_PIN                GPIO_PIN_8
#define I2C1_SCL_PORT               GPIOB
#define I2C1_SCL_AF                 GPIO_AF4_I2C1

#define I2C1_SDA_PIN                GPIO_PIN_9
#define I2C1_SDA_PORT               GPIOB
#define I2C1_SDA_AF                 GPIO_AF4_I2C1

// =============================================================================
// LCD Control Pins (directly from wiring guide)
// =============================================================================

#ifndef LCD_CS_PIN
#define LCD_CS_PIN                  GPIO_PIN_11
#endif
#ifndef LCD_CS_PORT
#define LCD_CS_PORT                 GPIOE
#endif

#ifndef LCD_RST_PIN
#define LCD_RST_PIN                 GPIO_PIN_10
#endif
#ifndef LCD_RST_PORT
#define LCD_RST_PORT                GPIOE
#endif

#ifndef LCD_DC_PIN
#define LCD_DC_PIN                  GPIO_PIN_9
#endif
#ifndef LCD_DC_PORT
#define LCD_DC_PORT                 GPIOE
#endif

#ifndef LCD_LED_PIN
#define LCD_LED_PIN                 GPIO_PIN_8
#endif
#ifndef LCD_LED_PORT
#define LCD_LED_PORT                GPIOE
#endif

// =============================================================================
// Touch Controller Pins
// =============================================================================

#ifndef CTP_RST_PIN
#define CTP_RST_PIN                 GPIO_PIN_7
#endif
#ifndef CTP_RST_PORT
#define CTP_RST_PORT                GPIOE
#endif

#ifndef CTP_INT_PIN
#define CTP_INT_PIN                 GPIO_PIN_6
#endif
#ifndef CTP_INT_PORT
#define CTP_INT_PORT                GPIOE
#endif

#define CTP_I2C_ADDR                0x38    // FT6336U 7-bit address

// =============================================================================
// SD Card Pin
// =============================================================================

#ifndef SD_CS_PIN
#define SD_CS_PIN                   GPIO_PIN_5
#endif
#ifndef SD_CS_PORT
#define SD_CS_PORT                  GPIOE
#endif

// =============================================================================
// GPIO Control Macros
// =============================================================================

// LCD Control
#define LCD_CS_LOW()                HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET)
#define LCD_CS_HIGH()               HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET)
#define LCD_DC_DATA()               HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET)
#define LCD_DC_CMD()                HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET)
#define LCD_RST_LOW()               HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET)
#define LCD_RST_HIGH()              HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET)
#define LCD_LED_ON()                HAL_GPIO_WritePin(LCD_LED_PORT, LCD_LED_PIN, GPIO_PIN_SET)
#define LCD_LED_OFF()               HAL_GPIO_WritePin(LCD_LED_PORT, LCD_LED_PIN, GPIO_PIN_RESET)

// Touch Control
#define CTP_RST_LOW()               HAL_GPIO_WritePin(CTP_RST_PORT, CTP_RST_PIN, GPIO_PIN_RESET)
#define CTP_RST_HIGH()              HAL_GPIO_WritePin(CTP_RST_PORT, CTP_RST_PIN, GPIO_PIN_SET)
#define CTP_INT_READ()              HAL_GPIO_ReadPin(CTP_INT_PORT, CTP_INT_PIN)

// SD Card Control
#define SD_CS_LOW()                 HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_RESET)
#define SD_CS_HIGH()                HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET)

// =============================================================================
// Clock Configuration
// =============================================================================

#define HSE_FREQUENCY               25000000U   // 25 MHz crystal on WeAct board
#define SYSCLK_FREQUENCY            480000000U  // 480 MHz system clock
#define HCLK_FREQUENCY              240000000U  // 240 MHz AHB clock
#define APB1_FREQUENCY              120000000U  // 120 MHz APB1
#define APB2_FREQUENCY              120000000U  // 120 MHz APB2

// =============================================================================
// SPI Speed Configuration
// =============================================================================

// SPI3 is on APB1 (120 MHz)
// Prescaler options: 2, 4, 8, 16, 32, 64, 128, 256
#define SPI_PRESCALER_SAFE          SPI_BAUDRATEPRESCALER_16    // 7.5 MHz (safe start)
#define SPI_PRESCALER_FAST          SPI_BAUDRATEPRESCALER_4     // 30 MHz (after testing)
#define SPI_PRESCALER_MAX           SPI_BAUDRATEPRESCALER_2     // 60 MHz (aggressive)

// =============================================================================
// DMA Configuration (for high-speed display transfers)
// =============================================================================

#define LCD_DMA                     DMA1
#define LCD_DMA_STREAM              DMA1_Stream0
#define LCD_DMA_CHANNEL             DMA_REQUEST_SPI3_TX
#define LCD_DMA_IRQ                 DMA1_Stream0_IRQn

#ifdef __cplusplus
}
#endif

#endif // BOARD_CONFIG_H
