#ifndef DISPLAY_H
#define DISPLAY_H

#include "stdlib.h"
#include "hardware_config.h"

constexpr uint16_t DISPLAY_HEIGHT = SCREEN_HEIGHT;
constexpr uint16_t DISPLAY_WIDTH = SCREEN_WIDTH;

void init_display();
void set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void send_data(uint8_t* data, size_t len);
void send_data_byte(uint8_t data);

#endif
