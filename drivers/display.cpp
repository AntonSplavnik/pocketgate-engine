#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware_config.h"
#include "display.h"

void initDisplay(){

	// Initialize SPI at 62.5 MHz
	spi_init(spi0, 62500000);

	// Set SPI format (8 bits, mode 0)
	spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

	// Assign pins to SPI function
	gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
	gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

	// Initialize Control Pins (CS, DC, RESET, BL)

	// Initialize CS (Chip Select)
	gpio_init(PIN_TFT_CS);
	gpio_set_dir(PIN_TFT_CS, GPIO_OUT);
	gpio_put(PIN_TFT_CS, 1);  // Start HIGH (not selected)

	// Initialize DC (Data/Command)
	gpio_init(PIN_DC);
	gpio_set_dir(PIN_DC, GPIO_OUT);

	// Initialize RESET
	gpio_init(PIN_RESET);
	gpio_set_dir(PIN_RESET, GPIO_OUT);
	gpio_put(PIN_RESET, 1);  // Start HIGH (not in reset)

	// Initialize Backlight
	gpio_init(PIN_BL);
	gpio_set_dir(PIN_BL, GPIO_OUT);
	gpio_put(PIN_BL, 1);  // Turn backlight ON

	// Hardware reset sequence
	gpio_put(PIN_RESET, 0);  // Pull RESET low
	sleep_ms(10);            // Wait 10ms
	gpio_put(PIN_RESET, 1);  // Pull RESET high
	sleep_ms(100);           // Wait for display to boot

}
