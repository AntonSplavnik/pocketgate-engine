#include "pico/stdlib.h"

#include "st7735_driver.h"
#include "display.h"
#include "framebuffer.h"

void Framebuffer::init() {

	fill_with_color(0x0000);
	Framebuffer::send_to_display();
}

void Framebuffer::swap_buffers() {

	uint16_t* buffer = front_buffer;
	front_buffer = back_buffer;
	back_buffer = buffer;
}

void Framebuffer::fill_with_color(uint16_t color) {

	uint16_t swapped_color = (color << 8) | (color >> 8);
	for (size_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
		back_buffer[i] = swapped_color;
	}
}

void Framebuffer::set_pixel(uint16_t x, uint16_t y, uint16_t color) {

	if(x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;
	back_buffer[y * DISPLAY_WIDTH + x] = (color << 8) | (color >> 8);
}

void Framebuffer::draw_line(uint16_t x, uint16_t y, uint16_t line_len, uint16_t color) {
	uint16_t swapped_color = (color << 8) | (color >> 8);
	for (size_t i = 0; i < line_len; i++) {
		Framebuffer::back_buffer[y * DISPLAY_WIDTH + x + i] = swapped_color;
	}
}

void Framebuffer::send_to_display() {

	set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
	uint16_t buffer_size = DISPLAY_WIDTH * DISPLAY_HEIGHT;
	send_data((uint8_t*)front_buffer, buffer_size * 2);
}



void color_test_nobuffer() {
	set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);  // Full screen

	// Black
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0x00);
		send_data_byte(0x00);
	}
	sleep_ms(5000);

	// White
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0xFF);
		send_data_byte(0xFF);
	}
	sleep_ms(5000);

	// Gray
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0x84);
		send_data_byte(0x10);
	}
	sleep_ms(5000);

	// Red
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0xF8);
		send_data_byte(0x00);
	}
	sleep_ms(5000);

	// Green
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0x07);
		send_data_byte(0xE0);
	}
	sleep_ms(5000);

	// Blue
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0x00);
		send_data_byte(0x1F);
	}
	sleep_ms(5000);

	// Yellow
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0xFF);
		send_data_byte(0xE0);
	}
	sleep_ms(5000);

	// Magenta
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0xF8);
		send_data_byte(0x1F);
	}
	sleep_ms(5000);

	// Cyan
	for (int i = 0; i < 20480; i++) {
		send_data_byte(0x07);
		send_data_byte(0xFF);
	}

	sleep_ms(5000);
}
