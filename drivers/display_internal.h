#ifndef DISPLAY_INTERNAL_H
#define DISPLAY_INTERNAL_H

#include "stdlib.h"

void display_toggle_test();
void color_test1();
void color_test2();
void init_display_commands();
void send_command(uint8_t cmd);
void send_data(uint8_t* data, size_t len);
void send_data_byte(uint8_t data);

#endif
