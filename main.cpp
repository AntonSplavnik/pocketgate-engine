#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "stdlib.h"
#include "random"
#include "drivers/display.h"

const uint LED_PIN = 25;
const uint LED_L = 28;
const uint LED_R = 4;

static std::mt19937 rng(time_us_32());
int randomInt(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

void init_led_pwm(uint pin) {
	gpio_set_function(pin, GPIO_FUNC_PWM);
	uint slice = pwm_gpio_to_slice_num(pin);
	pwm_set_wrap(slice, 1023);
	pwm_set_clkdiv(slice, 122.07f);
	pwm_set_gpio_level(pin, 0);
	pwm_set_enabled(slice, true);
}
void set_brightness_level(uint pin, uint16_t level) {
	if(level > 1023) level = 1023;
	pwm_set_gpio_level(pin, level);
}

void dim_led(uint led, uint16_t level) {
	for (size_t i = 0; i < level; i++)
	{
		set_brightness_level(led, i);
		sleep_ms(1);
	}
	for (int i = level; i >= 0; i--)
	{
		set_brightness_level(led, i);
		sleep_ms(1);
	}
}
void init_led_pins() {

	// Pico onboard LED
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	init_led_pwm(LED_L);
	init_led_pwm(LED_R);

	// // Trigg Led_left
	// gpio_init(LED_L);
	// gpio_set_dir(LED_L, GPIO_OUT);

	// // Trigg Led_right
	// gpio_init(LED_R);
	// gpio_set_dir(LED_R, GPIO_OUT);
}

void blik(){

	init_led_pins();

	while (true) {
		gpio_put(LED_PIN, 1);
		printf("LED ON!\n");
		sleep_ms(randomInt(0, 250));
		gpio_put(LED_PIN, 0);
		printf("LED OFF!\n");

		dim_led(LED_L, 1023);
		sleep_ms(randomInt(0, 250));
		dim_led(LED_R, 1023);
		sleep_ms(randomInt(0, 250));

		// gpio_put(LED_L, 1);
		// sleep_ms(randomInt(0, 250));
		// gpio_put(LED_L, 0);

		// gpio_put(LED_R, 1);
		// sleep_ms(randomInt(0, 250));
		// gpio_put(LED_R, 0);
	}
}

int main(){
	stdio_init_all();
	sleep_ms(1000);
	printf("TriggEngine v0.1\n");
	init_display();
	blik();
	return 0;
}
