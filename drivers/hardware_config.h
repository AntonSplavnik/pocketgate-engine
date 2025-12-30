#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// Led pins
#define PIN_LED_R   4
#define PIN_LED_L   28

// Buttons pins
#define BTN_W 5
#define BTN_A 6
#define BTN_S 7
#define BTN_D 8
#define BTN_I 12
#define BTN_J 13
#define BTN_K 14
#define BTN_L 15

// Sound pins
#define DIN         9             // Audio data stream
#define BCLK        10            // Clock signal (like SPI's SCK)
#define LRCLK       11            // Channel select (left/right stereo)

//Display pins
#define PIN_SCK     18            // SPI Clock
#define PIN_MOSI    19            // SPI Data Out
#define PIN_MISO    16            // SPI Data In (SD card only, not needed for display)

#define PIN_BL      17            // Backlight (LITE)
#define PIN_TFT_CS  20            // Display Chip Select
#define PIN_CARD_CS 21            // SD Card Chip Select
#define PIN_DC      22            // Data/Command (D/C)
#define PIN_RESET   26            // Display Reset

// Display settings
#define SCREEN_HEIGHT 128
#define SCREEN_WIDTH 160

#endif
