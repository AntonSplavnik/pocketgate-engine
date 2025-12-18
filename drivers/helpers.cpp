#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware_config.h"
#include "display.h"

void send_command(uint8_t cmd) {
gpio_put(PIN_TFT_CS, 0);
gpio_put(PIN_DC, 0);
spi_write_blocking(spi0, &cmd, 0);
gpio_put(PIN_TFT_CS, 1);
}

void send_data(uint8_t* data, size_t len) {
gpio_put(PIN_TFT_CS, 0);
gpio_put(PIN_DC, 1);
spi_write_blocking(spi0, data, len);
gpio_put(PIN_TFT_CS, 1);
}

void send_data_byte(uint8_t data) {
gpio_put(PIN_TFT_CS, 0);
gpio_put(PIN_DC, 1);
spi_write_blocking(spi0, &data, 1);
gpio_put(PIN_TFT_CS, 1);
}
