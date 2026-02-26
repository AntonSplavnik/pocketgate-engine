#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware_config.h"

// Hardware initialization
void init_SPI_bus() {
	// Initialize SPI at 62.5 MHz
	spi_init(spi0, 62500000);
	// Set SPI format (8 bits, mode 0)
	spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}
void init_SPI_pins() {
	gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
	gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
}
