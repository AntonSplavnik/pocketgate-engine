#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "display.h"

void blik(){

	const uint LED_PIN = 25;  // Onboard LED
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	while (true) {
		gpio_put(LED_PIN, 1);
		sleep_ms(500);
		gpio_put(LED_PIN, 0);
		sleep_ms(500);
	}
}


int main(){

	initDisplay();
	blik();
	return 0;
}
