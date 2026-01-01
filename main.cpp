#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "stdlib.h"
#include "random"
#include <math.h>

#include "drivers/display.h"
#include "framebuffer.h"

const uint LED_PIN = 25;
const uint LED_L = 28;
const uint LED_R = 4;

static std::mt19937 rng(time_us_32());
int random_int_distr(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}
int random_int_modulo(int min, int max) {
	return min + (rng() % (max - min + 1));
}

void init_led_pwm(uint pin) {
	gpio_set_function(pin, GPIO_FUNC_PWM);
	uint slice = pwm_gpio_to_slice_num(pin);
	pwm_set_wrap(slice, 1023);
	pwm_set_clkdiv(slice, 122.07f);
	pwm_set_gpio_level(pin, 0);
	pwm_set_enabled(slice, true);
}
void set_led_brightness(uint pin, uint16_t level) {
	if(level > 1023) level = 1023;
	pwm_set_gpio_level(pin, level);
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

void ambient_glow(uint led, float speed) {
	 float time_sec = time_us_64() / 1000000.0f;

	uint16_t brightness = (sinf(time_sec *  speed) + 1.0f) * 500.0f;
	set_led_brightness(led, brightness);
}
void dim_led(uint led, float speed ) {
	float time_sec = time_us_64() / 1000000.0f;

	// Triangle wave (0→1→0→1...)
	float cycle = fmodf(time_sec * speed, 2.0f);  // 2-second cycle
	float brightness_normalized = (cycle < 1.0f) ? cycle : (2.0f - cycle);

	uint16_t brightness = brightness_normalized * 1023.0f;
	set_led_brightness(led, brightness);
}
void dim_led(uint led, uint16_t level) {
	for (size_t i = 0; i < level; i++)
	{
		set_led_brightness(led, i);
		sleep_ms(1);
	}
	for (int i = level; i >= 0; i--)
	{
		set_led_brightness(led, i);
		sleep_ms(1);
	}
}

void blik(){

	init_led_pins();

	while (true) {
		// Onboard LED blinks
		// gpio_put(LED_PIN, 1);
		// sleep_ms(randomInt(0, 250));
		// gpio_put(LED_PIN, 0);

		// Left LED
		// dim_led(LED_L, 1023);
		// sleep_ms(randomInt(0, 250));

		// gpio_put(LED_L, 1);
		// sleep_ms(randomInt(0, 250));
		// gpio_put(LED_L, 0);

		// Right LED
		// sleep_ms(randomInt(0, 250));

		// gpio_put(LED_R, 1);
		// sleep_ms(randomInt(0, 250));
		// gpio_put(LED_R, 0);

		dim_led(LED_L, 0.5f);
		ambient_glow(LED_R, 1.0f);
		sleep_ms(10);
	}
}

struct NamedColor {
	const char* name;
	uint16_t value;
};

static const NamedColor COLORS[] = {
	{"RED",     0xF800},
	{"GREEN",   0x07E0},
	{"BLUE",    0x001F},
	{"WHITE",   0xFFFF},
	{"BLACK",   0x0000},
	{"YELLOW",  0xFFE0},
	{"CYAN",    0x07FF},
	{"MAGENTA", 0xF81F}
};

void color_test() {

	Framebuffer::init();
	sleep_ms(3000);

	for(const auto& color: COLORS) {
		Framebuffer::fill_with_color(color.value);
		Framebuffer::swap_buffers();
		Framebuffer::send_to_display();
		sleep_ms(3000);
	}

}

void fps_counter() {

	static uint32_t frame_count = 0;
	static uint64_t last_frame_time = time_us_64();

	frame_count++;

	uint64_t elapsed_time = time_us_64() - last_frame_time;
	if(elapsed_time >= 1000000) {
		printf("FPS: %lu\n", frame_count);
		frame_count = 0;
		last_frame_time = time_us_64();
	}
}

void random_pixels_test() {
	while(true) {
		Framebuffer::fill_with_color(0x0000);  // Black background
		// Draw complex scene to back buffer
		for(int i = 0; i < 5000; i++) {
			Framebuffer::set_pixel(random_int_modulo(0, 160), random_int_modulo(0, 128), COLORS[random_int_modulo(5, 7)].value);
		}

		fps_counter();

		Framebuffer::swap_buffers();
		Framebuffer::send_to_display();
		// No sleep - immediate next frame!
	}
}
void line_test(){
	Framebuffer::fill_with_color(0x0000);
	Framebuffer::draw_line(50, 50, 50, 0xFFE0);
	Framebuffer::swap_buffers();
	Framebuffer::send_to_display();
}

void rectangle_test(uint16_t start_raw_y, uint16_t number_of_raws_y, uint16_t x, uint16_t line_len, uint16_t color){
	if(start_raw_y > SCREEN_HEIGHT - 1) start_raw_y = SCREEN_HEIGHT - 1;
	if(number_of_raws_y > SCREEN_HEIGHT - 1) number_of_raws_y = SCREEN_HEIGHT - 1;

	Framebuffer::fill_with_color(0x0000);
	for (size_t y = start_raw_y; y < start_raw_y+number_of_raws_y; y++)
	{
		Framebuffer::draw_line(x, y, line_len, color);
	}
	Framebuffer::swap_buffers();
	Framebuffer::send_to_display();
}

int main(){
	stdio_init_all();
	sleep_ms(3000);
	printf("TriggEngine v0.1\n");
	init_display();
	// random_pixels_test();
	// line_test();
	rectangle_test(128/2 - 25/2, 25, 160/2 - 25/2, 25, 0xFFE0);
	// color_test();
	blik();
	return 0;
}
