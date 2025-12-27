#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware_config.h"
#include "display.h"
#include "display_internal.h"
#include "spi.h"

// Low-level SPI communication
void send_command(uint8_t cmd) {
	gpio_put(PIN_TFT_CS, 0);
	gpio_put(PIN_DC, 0);
	spi_write_blocking(spi0, &cmd, 1);
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

// GPIO helper
void init_pin(int pin, enum gpio_dir GPIO, int voltage) {
	gpio_init(pin);
	gpio_set_dir(pin, GPIO);
	gpio_put(pin, voltage);
}

void init_control_pins_as_GPIO() {
	// Initialize CS (Chip Select)
	init_pin(PIN_TFT_CS, GPIO_OUT, 1); // Start HIGH (not selected)
	// Initialize DC (Data/Command)
	init_pin(PIN_DC, GPIO_OUT, 0);
	// Initialize RESET
	init_pin(PIN_RESET, GPIO_OUT, 1); // Start HIGH (not in reset)
	// Initialize Backlight
	init_pin(PIN_BL, GPIO_OUT, 1); // Turn backlight ON
}

// Hardware reset sequence
void reset_display() {
	gpio_put(PIN_RESET, 0);  // Pull RESET low
	sleep_ms(10);            // Wait 10ms
	gpio_put(PIN_RESET, 1);  // Pull RESET high
	sleep_ms(120);           // Wait for display to boot
}


// Main display initialization
void init_display(){
	init_SPI_bus();
	init_SPI_pins();
	init_control_pins_as_GPIO();
	reset_display();
	init_display_commands();

	color_test1();
	sleep_ms(1000);
	color_test2();
}

